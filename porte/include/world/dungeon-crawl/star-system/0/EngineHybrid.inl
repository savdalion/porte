namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;



inline EngineHybrid::EngineHybrid(
    real_t timestep
) :
    EngineWithoutBooster< pns::Portulan, pns::real_t > ( timestep ),

    // #! Если память выделена динамически, работаем с содержанием структуры.
    memsizeStarSystem( sizeof( pns::aboutStarSystem_t ) ),
    memsizeAsteroid( sizeof( pns::aboutAsteroid_t ) * pns::ASTEROID_COUNT ),
    memsizePlanet( sizeof( pns::aboutPlanet_t ) * pns::PLANET_COUNT ),
    memsizeStar( sizeof( pns::aboutStar_t ) * pns::STAR_COUNT ),

    errorCL( CL_SUCCESS ),
    devicesCL( nullptr ),
    deviceUsedCL( 0 ),
    devCountCL( 0 ),
    platformCL( nullptr ),
    gpuContextCL( nullptr ),
    commandQueueCL( nullptr )
{
    // Подготавливаем контекст и очередь команд для работы с OpenCL
    prepareCLContext();
    prepareCLCommandQueue();

    assert( gpuContextCL
        && "Контекст OpenCL не инициализирован." );
    assert( commandQueueCL
        && "Очередь команд OpenCL не инициализирована." );


    // Подготавливаем ядра OpenCL (ядра требуют компиляции)
    prepare();
}




inline EngineHybrid::~EngineHybrid() {
    // удаляем собранные ядра
    for (auto itr = kernelCL.begin(); itr != kernelCL.end(); ++itr) {
        errorCL |= clReleaseKernel( itr->second );
    }
#ifdef _DEBUG
    oclCheckError( errorCL, CL_SUCCESS );
#endif

    // освобождаем очередь команд и контекст
    clReleaseCommandQueue( commandQueueCL );
    clReleaseContext( gpuContextCL );
}




inline void EngineHybrid::pulse( int n ) {
    // выполняем 'n' циклов
    for (int i = 0; i < n; ++i) {
        pulse();
    }

    // вычисляем оптим. шаг времени, *предполагая*, что кол-во
    // пульсов будет тем же
    /* - @todo Время получается слишком малым. Разобраться.
    calcOptimalTimestep( n );
    */
}






inline void EngineHybrid::pulse() {

    assert( !mPortulan.expired() );

    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // рассчитываем воздействия на тела звёздной системы
    // # Все элементы звёздной системы работают по единой схеме.
    // # Каждый элемент звёздной системы генерирует свой набор событий
    //   согласно списку *_EVENT.
    // # Испускаемые элементом события хранятся в памяти событий у самого
    //   элемента.
    // # Событие может изменять неск. элементов. Поэтому, после "испускания"
    //   события каждый элемент просматривает др. элементы на наличие событий,
    //   которые могут оказать влияние на сам элемент.

    // сбрасываем память событий у элементов
    // @todo forgetEvent();

    // проходим последовательно по всем событиям элементов, пока не достигнем
    // стадии, когда ни один элемент не будет испускать новые события
    while ( emitEvent() ) {
        // просматриваем события выпущеные др. элементами,
        // регистрируем все симметричные события у себя
        // @todo observeOtherAndEmitEvent();
    }

    // уведомляем подписчиков "до"
    // @todo notifyBeforeEvent();

    // отрабатываем события, зарегистрированные у элементов
    // @todo completeEvent();

    // уведомляем подписчиков "после"
    // @todo notifyAfterEvent();


    // мир становится старше
    mLive.inc( mTimestep );

    // пульс пройден
    // @todo completePulse();


    // собираем статистику для элементов портулана
    statistics();
}




inline bool EngineHybrid::emitEvent() {

#ifdef _DEBUG
    std::cout << "Излучаем события ..";
#endif

    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    const cl_mem aboutStarSystemCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStarSystem,
        // #! Если память выделена динамически, обращаемся к содержанию.
        &topology.aboutStarSystem,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem asteroidCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeAsteroid,
        // #! Если память выделена динамически, обращаемся к содержанию.
        topology.asteroid.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem planetCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizePlanet,
        // #! Если память выделена динамически, обращаемся к содержанию.
        topology.planet.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem starCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStar,
        // #! Если память выделена динамически, обращаемся к содержанию.
        topology.star.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = typelib::max(
        pns::ASTEROID_COUNT, pns::PLANET_COUNT, pns::STAR_COUNT
    );
    static const size_t GRID_WORK_DIM = 1;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid };
    //static const size_t GRID_LOCAL_WORK_SIZE[] = { grid };

    // Излучаем события
    //{
        const cl_kernel kernel = kernelCL[ "set/emit-event/emit" ];

        errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &aboutStarSystemCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 1, sizeof( const cl_mem ), &asteroidCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 2, sizeof( const cl_mem ), &planetCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 3, sizeof( const cl_mem ), &starCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clEnqueueNDRangeKernel(
            commandQueueCL,
            kernel,
            GRID_WORK_DIM,
            nullptr,
            GRID_GLOBAL_WORK_SIZE,
            //GRID_LOCAL_WORK_SIZE,
            nullptr,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    //}

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        asteroidCL,
        CL_TRUE,
        0,
        memsizeAsteroid,
        topology.asteroid.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        planetCL,
        CL_TRUE,
        0,
        memsizePlanet,
        topology.planet.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        starCL,
        CL_TRUE,
        0,
        memsizeStar,
        topology.star.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    clReleaseMemObject( aboutStarSystemCL );
    clReleaseMemObject( asteroidCL );
    clReleaseMemObject( planetCL );
    clReleaseMemObject( starCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif

    return false;
}




inline bool EngineHybrid::forceGravityBodyAImpactIn(
    real_t force[ 3 ],
    const pns::aboutBody_t& a,
    const pns::aboutBody_t& b,
    real_t noForceDistance
) {
    // расстояние
    real_t r[ 3 ] = {
        b.coord[ 0 ] - a.coord[ 0 ],
        b.coord[ 1 ] - a.coord[ 1 ],
        b.coord[ 2 ] - a.coord[ 2 ],
    };
    const real_t distance2 = (
        r[ 0 ] * r[ 0 ] +
        r[ 1 ] * r[ 1 ] +
        r[ 2 ] * r[ 2 ]
    );
    if (distance2 < (noForceDistance * noForceDistance)) {
        // при столкновениях отключаем силу
        return false;
    }
    const real_t distance = sqrt( distance2 );
    if (distance > 0.0) {
        const real_t inv = 1.0 / distance;
        r[ 0 ] *= inv;
        r[ 1 ] *= inv;
        r[ 2 ] *= inv;
    }

    // сила гравитации
    const real_t f =
        typelib::constant::physics::G * a.mass /
        distance2 * b.mass;

    force[ 0 ] += r[ 0 ] * f;
    force[ 1 ] += r[ 1 ] * f;
    force[ 2 ] += r[ 2 ] * f;

    return true;
}




inline void EngineHybrid::statistics() {
    const auto& os = mPortulan.lock()->orderStatistics();
    for (auto itr = os.cbegin(); itr != os.cend(); ++itr) {
        itr->second->grabPulse( mLive.pulselive() );
    }
}




inline void EngineHybrid::prepareCLContext() {

    // @source http://nvidia.com/content/cuda/cuda-downloads.html / oclMarchingCubes.cpp

#ifdef ONLY_CPU_OPENCL_STARSYSTEM_L0_ENGINE_PORTE
    errorCL = getPlatformIDCPU( &platformCL );
    const static cl_device_type DEVICE_TYPE = CL_DEVICE_TYPE_CPU;
#else
    errorCL = oclGetPlatformID( &platformCL );
    const static cl_device_type DEVICE_TYPE = CL_DEVICE_TYPE_GPU;
#endif
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // Get the number of GPU devices available to the platform
    errorCL = clGetDeviceIDs(
        platformCL,
        DEVICE_TYPE,
        0,
        nullptr,
        &devCountCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // Create the device list
    devicesCL = new cl_device_id[ devCountCL ];
    errorCL = clGetDeviceIDs(
        platformCL,
        DEVICE_TYPE,
        devCountCL,
        devicesCL,
        nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    size_t endDevCL = devCountCL - 1;
    deviceUsedCL = CLAMP( deviceUsedCL, 0, endDevCL );
    endDevCL = deviceUsedCL;

    // No GL interop
    const cl_context_properties props[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformCL, 0 };
    // (!) Если установлен слишком большой размер стека, OpenCL не будет инициализирован.
    //     Включение LARGEADDRESSAWARE не решает проблему.
    gpuContextCL = clCreateContext(
        props, 1, &devicesCL[ deviceUsedCL ],
        &pfn_notify_cl, nullptr, &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "Выбрано устройство (OpenCL):" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif

}





inline void EngineHybrid::prepareCLCommandQueue() {
    assert( gpuContextCL
        && "Контекст OpenCL требуется инициализировать до выполнения этого метода." );

    // create a command-queue
    commandQueueCL =
        clCreateCommandQueue( gpuContextCL, devicesCL[ deviceUsedCL ], 0, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}





inline void EngineHybrid::compileCLKernel(
    const std::vector< std::string >&  kernelKeys,
    const std::vector< std::string >&  includeHCL,
    const std::string& options
) {
    // # Контекст и очередь команд инициализированы в конструкторе.

#ifdef _DEBUG
        std::cout << std::endl << std::endl;
#endif

    std::ostringstream commonOptions;
    commonOptions
        // лечим точность для float
        << std::fixed
        // #! Рабочие сетки декларируются отдельно: есть методы
        //   в helper.hcl, которые используют их.
        << " " << options
        << commonConstantCLKernel()
        << commonOptionCLKernel()
        << "";

    // здесь будет код необходимых ядру библиотек
    std::string kernelLibraryCode = "";

    // подключаем общие библиотеки и структуры
    // (используются всеми ядрами движка)
    // #! Важен порядок подключения.
    std::vector< std::string > hcl = boost::assign::list_of
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/set/pragma.hcl" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/set/restruct.hcl" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/structure.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/asteroid.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/planet.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/star.h" )
        // все h-файлы выше включены, т.к. они включены в
        // star-system.h (используется во всех ядрах)
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/star-system.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/utils.h" )
    ;
    hcl.insert( hcl.end(), includeHCL.cbegin(), includeHCL.cend() );
    for (auto itr = hcl.cbegin(); itr != hcl.cend(); ++itr) {
        const std::string& pathAndName = *itr;
#ifdef _DEBUG
        const std::string shortName =
            itr->substr( pathAndName.find_last_of( '/' ) + 1 );
        std::cout << "Собираем \"" << shortName << "\" ..";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        assert( file.is_open()
            && "Файл не найден." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        kernelLibraryCode +=
            (std::string)"\n\n\n\n\n" +
            "// @include " + pathAndName + "\n" +
            buffer.str();
#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
    } // for (auto itr = hcl.cbegin(); ...


    // компилируем ядро
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # Последнее за "/" название является именем ядра.
        const std::string kernelKey = *itr;
        const std::string kernelName = itr->substr( itr->find_last_of( '/' ) + 1 );

        // Program Setup
        const std::string fileKernel = kernelKey + ".cl";
        const std::string pathAndName =
            L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/" + fileKernel;
#ifdef _DEBUG
        std::cout << "Собираем \"" << fileKernel << "\" ..";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        assert( file.is_open()
            && "Файл ядра не найден." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        const std::string kernelSC = buffer.str();
#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif

        // create the program
        const std::string kernelSourceCode =
            kernelLibraryCode + "\n\n\n\n\n" +
            "// @include kernel " + pathAndName + "\n" +
            kernelSC;

#ifdef _DEBUG
        // сохраняем полный код ядра в файл
        const std::string fn =
            boost::replace_all_copy( kernelKey, "/", "_" ) + ".debug.cl";
        std::ofstream  out( fn.c_str() );
        out << kernelSourceCode;
        out.close();
#endif

        const char* programSource = kernelSourceCode.c_str();
        const size_t programLength = kernelSourceCode.length();
        cl_program programCL = clCreateProgramWithSource(
            gpuContextCL,  1,  &programSource,
            &programLength, &errorCL
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    
#ifdef _DEBUG
        std::cout << "Опции OpenCL для ядра \"" << kernelKey << "\"" << std::endl << commonOptions.str() << std::endl;
#endif

        // build the program
        errorCL = clBuildProgram(
            programCL, 0, nullptr, commonOptions.str().c_str(),
            //pfn_notify_program_cl, nullptr
            nullptr, nullptr
        );
        if (errorCL != CL_SUCCESS) {
            shrLogEx( LOGCONSOLE | ERRORMSG, errorCL, STDERROR );
            oclLogBuildInfo( programCL, oclGetFirstDev( gpuContextCL ) );
            const std::string debugFileKernel = kernelName + ".ptx";
            oclLogPtx( programCL, oclGetFirstDev( gpuContextCL ), debugFileKernel.c_str() );
            fnErrorCL( errorCL );
        }

        // create the kernel
        cl_kernel oneKernelCL =
            clCreateKernel( programCL, kernelName.c_str(), &errorCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        kernelCL[ kernelKey ] = oneKernelCL;

        // Некоторые ресурсы надо освободить прямо сейчас
        clReleaseProgram( programCL );

    } // for (auto itr

}





inline std::string EngineHybrid::commonConstantCLKernel() {

    std::ostringstream options;
    options
        // лечим точность для float
        << std::fixed

#ifdef PERMIT_DOUBLE_ENGINE_PORTE
        // используем double вместо float
        // @see pns::real_t
        << " -D PERMIT_DOUBLE_ENGINE_PORTE"
#endif

        << " -D ASTEROID_COUNT=" << pns::ASTEROID_COUNT
        << " -D PLANET_COUNT=" << pns::PLANET_COUNT
        << " -D STAR_COUNT=" << pns::STAR_COUNT
        << " -D EMITTER_EVENT_COUNT=" << pns::EMITTER_EVENT_COUNT
        << " -D MAX_FEATURE_EVENT=" << pns::MAX_FEATURE_EVENT

        // точность сравнения значений с плав. точкой
        << " -D PRECISION=" << typelib::PRECISION

        // физические константы
        << std::scientific
        // перевод из Кельвина в Цельсий
        << " -D CK_TEMPERATURE=" << typelib::constant::physics::CK
        // гравитационная постоянная
        << " -D G=" << typelib::constant::physics::G

        << "";

    return options.str();
}





inline std::string EngineHybrid::commonOptionCLKernel() {
    std::ostringstream options;
    options
        // лечим точность для float
        //<< std::fixed

        // предупреждения в ядрах считаем ошибками
        << " -Werror"
#if 0
        // серьёзная оптимизация
        // (i) ~10% прирост только с включением опций ниже.
        // @todo optimize Тонкая оптимизация OpenCL.
        //       http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
        << " -cl-fast-relaxed-math"
        << " -cl-mad-enable"
#endif
#if 0
        // внимательная отладка
        // (i) Включать только при вылавливании блох: более чем
        //     20-кратное замедление.
        << " -cl-opt-disable"
#endif
        << "";

    return options.str();
}





inline void EngineHybrid::fnErrorCL( int exitCode ) {
    std::cerr << "Код ошибки OpenCL: " << exitCode << std::endl;

    // @todo fine Выбрасывать исключение.
    std::cin.ignore();
    std::exit( exitCode );
}








inline void __stdcall pfn_notify_cl(
    const char* errinfo, const void* private_info,
    size_t cb, void* user_data
) {
    std::cout << std::endl << "(!) OpenCL error via pfn_notify_cl(): " << *errinfo << std::endl;
};



/*
inline void __stdcall pfn_notify_program_cl(
    cl_program, void* user_data
) {
    fprintf( stderr, "\n(!) OpenCL error via pfn_notify_program_cl().\n" );
};
*/





inline cl_int getPlatformIDCPU( cl_platform_id* clSelectedPlatformID ) {
    char chBuffer[1024];
    cl_uint num_platforms; 
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;
    *clSelectedPlatformID = NULL;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
    {
        shrLog(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        return -1000;
    }
    else 
    {
        if(num_platforms == 0)
        {
            shrLog("No OpenCL platform found!\n\n");
            return -2000;
        }
        else 
        {
            // if there's a platform or more, make space for ID's
            if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
            {
                shrLog("Failed to allocate memory for cl_platform ID's!\n\n");
                return -3000;
            }

            // get platform info for each platform and trap the CPU platform if found
            ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
            for(cl_uint i = 0; i < num_platforms; ++i)
            {
                ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if(ciErrNum == CL_SUCCESS)
                {
                    if ( (strstr( chBuffer, "Intel" ) != NULL)
                      || (strstr( chBuffer, "AMD"   ) != NULL)
                    ) {
                        *clSelectedPlatformID = clPlatformIDs[i];
                        break;
                    }
                }
            }

            // default to zeroeth platform if NVIDIA not found
            if(*clSelectedPlatformID == NULL)
            {
                shrLog("WARNING: CPU OpenCL platform not found - defaulting to first platform!\n\n");
                *clSelectedPlatformID = clPlatformIDs[0];
            }

            free(clPlatformIDs);
        }
    }

    return CL_SUCCESS;
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
