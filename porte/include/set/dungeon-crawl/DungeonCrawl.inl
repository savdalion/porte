namespace porte {


inline DungeonCrawl::DungeonCrawl(
    portulan_t* p
) :
    EngineWithoutBooster( p ),

    aboutPlanetCL( nullptr ),
    memsizeAboutPlanet( sizeof( portulan::planet::set::dungeoncrawl::aboutPlanet_t ) ),

    componentCL( nullptr ),
    workComponentCL( nullptr ),
    // #! Если память выделена динамически, работаем с содержанием структуры.
    memsizeComponent( sizeof( portulan::planet::set::dungeoncrawl::component::componentCell_t ) *
        portulan::planet::set::dungeoncrawl::COMPONENT_GRID *
        portulan::planet::set::dungeoncrawl::COMPONENT_GRID *
        portulan::planet::set::dungeoncrawl::COMPONENT_GRID
    ),

    livingCL( nullptr ),
    workLivingCL( nullptr ),
    memsizeLiving( sizeof( portulan::planet::set::dungeoncrawl::living::livingCell_t ) *
        portulan::planet::set::dungeoncrawl::LIVING_GRID *
        portulan::planet::set::dungeoncrawl::LIVING_GRID *
        portulan::planet::set::dungeoncrawl::LIVING_GRID
    ),

    temperatureCL( nullptr ),
    workTemperatureCL( nullptr ),
    memsizeTemperature( sizeof( portulan::planet::set::dungeoncrawl::temperature::temperatureCell_t ) *
        portulan::planet::set::dungeoncrawl::TEMPERATURE_GRID *
        portulan::planet::set::dungeoncrawl::TEMPERATURE_GRID *
        portulan::planet::set::dungeoncrawl::TEMPERATURE_GRID
    ),

    // проинициализируем генератор случ. чисел
    randomCore(),
    randomDistribution( 0, 0xFFFFFFFF ),
    randomGenerator( randomCore, randomDistribution ),
    randomSeed( 131071 ),

    errorCL( CL_SUCCESS ),
    devicesCL( nullptr ),
    deviceUsedCL( 0 ),
    devCountCL( 0 ),
    platformCL( nullptr ),
    gpuContextCL( nullptr ),
    commandQueueCL( nullptr )
{
    namespace pd = portulan::planet::set::dungeoncrawl;

    // Подготавливаем контекст и очередь команд для работы с OpenCL
    prepareCLContext();
    prepareCLCommandQueue();

    assert( gpuContextCL
        && "Контекст OpenCL не инициализирован." );
    assert( commandQueueCL
        && "Очередь команд OpenCL не инициализирована." );


    // Подготавливаем стат. структуры для ядер OpenCL

    // aboutPlanet
    aboutPlanetCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeAboutPlanet,
        &mPortulan->topology().topology().aboutPlanet,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // component
    componentCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeComponent,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    workComponentCL = clCreateBuffer(
        gpuContextCL,
        CL_MEM_READ_WRITE,
        memsizeComponent,
        nullptr,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // living
    livingCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLiving,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().living.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    workLivingCL = clCreateBuffer(
        gpuContextCL,
        CL_MEM_READ_WRITE,
        memsizeLiving,
        nullptr,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // temperature
    temperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    workTemperatureCL = clCreateBuffer(
        gpuContextCL,
        CL_MEM_READ_WRITE,
        memsizeTemperature,
        nullptr,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // Подготавливаем ядра OpenCL (ядра требуют компиляции)
    prepareCLKernel();


    // Установим зерно для генератора случ. чисел
    randomCore.seed( randomSeed );
}





inline DungeonCrawl::~DungeonCrawl() {
    /* - Нет: отдаём для OpenCL ссылки на созданные извне структуры...
    // освобождаем структуры для обмена данными с OpenCL
    clReleaseMemObject( aboutPlanetCL );
    ...
    */
    // ...но временные структуры - исключение
    clReleaseMemObject( workComponentCL );
    clReleaseMemObject( workLivingCL );
    clReleaseMemObject( workTemperatureCL );

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






inline void DungeonCrawl::pulse( int n ) {
    assert( false && "Не реализовано." );
}






inline void DungeonCrawl::prepareCLContext() {

    // @source http://nvidia.com/content/cuda/cuda-downloads.html / oclMarchingCubes.cpp

    //Get the platform
    errorCL = oclGetPlatformID( &platformCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // Get the number of GPU devices available to the platform
    errorCL = clGetDeviceIDs( platformCL, CL_DEVICE_TYPE_GPU, 0, nullptr, &devCountCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // Create the device list
    devicesCL = new cl_device_id[ devCountCL ];
    errorCL = clGetDeviceIDs( platformCL, CL_DEVICE_TYPE_GPU, devCountCL, devicesCL, nullptr );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    size_t endDevCL = devCountCL - 1;
    deviceUsedCL = CLAMP( deviceUsedCL, 0, endDevCL );
    endDevCL = deviceUsedCL;

    // No GL interop
    const cl_context_properties props[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformCL, 0 };
    // (!) Если установлен слишком большой размер стека, OpenCL не будет инициализирован.
    //     Включение LARGEADDRESSAWARE не решает проблему.
    gpuContextCL = clCreateContext( props, 1, &devicesCL[ deviceUsedCL ], nullptr, nullptr, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "Выбрано устройство (OpenCL):" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif

}





inline void DungeonCrawl::prepareCLCommandQueue() {
    assert( gpuContextCL
        && "Контекст OpenCL требуется инициализировать до выполнения этого метода." );

    // create a command-queue
    commandQueueCL = clCreateCommandQueue( gpuContextCL, devicesCL[ deviceUsedCL ], 0, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}





template< size_t G >
inline void DungeonCrawl::compileCLKernel(
    const std::vector< std::string >&  kernelKeys,
    const std::string& options
) {
    // # Контекст и очередь команд инициализированы в конструкторе.

    typedef typelib::StaticMapContent3D< G, G, G >  smc_t;

    // масштаб для рабочей сетки считаем здесь, чтобы не нагружать ядра
    // # Масштаб - сколько метров содержит 1 ячейка рабочей сетки.
    const float scale =
        mPortulan->topology().topology().aboutPlanet.size /
        static_cast< float >( G );

    std::ostringstream commonOptions;
    commonOptions
        // лечим точность для float
        << std::fixed
        // #! Рабочие сетки декларируются отдельно: есть методы
        //   в helper.hcl, которые используют их.
        << " -D GRID=" << G
        << " -D MIN_COORD_GRID=" << smc_t::minCoord().x
        << " -D MAX_COORD_GRID=" << smc_t::maxCoord().x
        << " -D SCALE=" << scale
        << " " << options
        << commonConstantCLKernel()
        << commonOptionCLKernel()
        << "";

    // @todo Искать по папкам в "scale". Сейчас - фиксированный путь в
    //       kernelKeys, плохо.
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # Последнее за "/" название является именем ядра.
        const std::string kernelKey = *itr;
        const std::string kernelName = itr->substr( itr->find_last_of( '/' ) + 1 );

        // Program Setup
        size_t programLength;
        const std::string fileKernel = kernelKey + ".cl";
        const std::string pathAndName = PATH_CL_DUNGEONCRAWL + "/" + fileKernel;
        const char* pureSourceCode = oclLoadProgSource( pathAndName.c_str(), "", &programLength );
        oclCheckErrorEX( (pureSourceCode != nullptr), true, &fnErrorCL );

        // create the program
        cl_program programCL = clCreateProgramWithSource(
            gpuContextCL,  1,  (const char**)&pureSourceCode,  &programLength, &errorCL
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    
#ifdef _DEBUG
        // @test
        std::cout << std::endl << "Опции OpenCL для ядра \"" << kernelKey << "\"" << std::endl << commonOptions.str() << std::endl;
#endif

        // build the program
        errorCL = clBuildProgram( programCL, 0, nullptr, commonOptions.str().c_str(), nullptr, nullptr );
        if (errorCL != CL_SUCCESS) {
            shrLogEx( LOGCONSOLE | ERRORMSG, errorCL, STDERROR );
            oclLogBuildInfo( programCL, oclGetFirstDev( gpuContextCL ) );
            const std::string debugFileKernel = kernelName + ".ptx";
            oclLogPtx( programCL, oclGetFirstDev( gpuContextCL ), debugFileKernel.c_str() );
            fnErrorCL( errorCL );
        }

        // create the kernel
        cl_kernel oneKernelCL = clCreateKernel( programCL, kernelName.c_str(), &errorCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        kernelCL[ kernelKey ] = oneKernelCL;

        // Некоторые ресурсы надо освободить прямо сейчас
        clReleaseProgram( programCL );

    } // for (auto itr

}





inline std::string DungeonCrawl::commonConstantCLKernel() {
    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pc = portulan::planet::set::dungeoncrawl::component;
    namespace pl = portulan::planet::set::dungeoncrawl::living;
    namespace pt = portulan::planet::set::dungeoncrawl::temperature;

    static const std::string randstamp =
        boost::lexical_cast< std::string >( static_cast< unsigned int >( time( nullptr ) ) );

    // структуры для вычисления минимаксов координат для сеток
    typedef typelib::StaticMapContent3D< pd::COMPONENT_GRID, pd::COMPONENT_GRID, pd::COMPONENT_GRID >        componentSMC_t;
    typedef typelib::StaticMapContent3D< pd::LIVING_GRID, pd::LIVING_GRID, pd::LIVING_GRID >                 livingSMC_t;
    typedef typelib::StaticMapContent3D< pd::TEMPERATURE_GRID, pd::TEMPERATURE_GRID, pd::TEMPERATURE_GRID >  temperatureSMC_t;

    std::ostringstream options;
    options
        // лечим точность для float
        << std::fixed

        // component
        << " -D COMPONENT_GRID=" << pd::COMPONENT_GRID
        << " -D MIN_COORD_COMPONENT_GRID=" << componentSMC_t::minCoord().x
        << " -D MAX_COORD_COMPONENT_GRID=" << componentSMC_t::maxCoord().x
        << " -D COMPONENT_COUNT=" << pd::COMPONENT_COUNT
        << " -D COMPONENT_CELL=" << pd::COMPONENT_CELL

        // living
        << " -D LIVING_GRID=" << pd::LIVING_GRID
        << " -D MIN_COORD_LIVING_GRID=" << componentSMC_t::minCoord().x
        << " -D MAX_COORD_LIVING_GRID=" << componentSMC_t::maxCoord().x
        << " -D LIVING_COUNT=" << pd::LIVING_COUNT
        << " -D LIVING_CELL=" << pd::LIVING_CELL
        << " -D LIFE_CYCLE_COUNT=" << pd::LIFE_CYCLE_COUNT
        << " -D PART_LIVING_COUNT=" << pd::PART_LIVING_COUNT
        << " -D ATTACK_PART_LIVING_COUNT=" << pd::ATTACK_PART_LIVING_COUNT
        << " -D RESIST_PART_LIVING_COUNT=" << pd::RESIST_PART_LIVING_COUNT
        << " -D HABITAT_LIVING_COUNT=" << pd::HABITAT_LIVING_COUNT
        << " -D COMPONENT_COMPOSITION_LIVING=" << pd::COMPONENT_COMPOSITION_LIVING
        << " -D COMPONENT_NEED_LIVING=" << pd::COMPONENT_NEED_LIVING
        << " -D COMPONENT_WASTE_LIVING=" << pd::COMPONENT_WASTE_LIVING
        << " -D ENERGY_NEED_LIVING=" << pd::ENERGY_NEED_LIVING
        << " -D ENERGY_WASTE_LIVING=" << pd::ENERGY_WASTE_LIVING
        << " -D ENVIRONMENT_SURVIVOR_LIVING=" << pd::ENVIRONMENT_SURVIVOR_LIVING
        << " -D COMPONENT_COMPOSITION_LIVING=" << pd::COMPONENT_COMPOSITION_LIVING

        // temperature
        << " -D TEMPERATURE_GRID=" << pd::TEMPERATURE_GRID
        << " -D MIN_COORD_TEMPERATURE_GRID=" << componentSMC_t::minCoord().x
        << " -D MAX_COORD_TEMPERATURE_GRID=" << componentSMC_t::maxCoord().x

        // точность сравнения значений с плав. точкой
        << " -D PRECISION=" << typelib::PRECISION

        // физические константы
        // перевод из Кельвина в Цельсий
        << " -D CK_TEMPERATURE=" << typelib::constant::physics::CK
        /* - @todo ...
        // Газовая постоянная, Дж / (Моль * K)
        // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
        << " -D R_GAS=8.31441f"
        */
#ifdef ALWAYS_BUILD_CL_KERNEL_PORTE
        // добавляем к настройкам (в 2012 г. драйвер OpenCL от NVIDIA
        // научился хешировать файл без учёта комментариев) уникальный
        // хвост, чтобы OpenCL не думал брать построенную ранее программу
        // из кеша устройства: иначе рискуем получать феноменальные ошибки,
        // когда изменения во *включаемых файлах* учитываются "через раз"
        << " -D BUILD_RANDSTAMP=" << randstamp
#endif
        << "";

    return options.str();
}





inline std::string DungeonCrawl::commonOptionCLKernel() {
    std::ostringstream options;
    options
        // лечим точность для float
        //<< std::fixed

        // путь к включаемым файлам (*.hcl)
        << " -I " << PATH_CL_DUNGEONCRAWL
        // предупреждения в ядрах считаем ошибками
        << " -Werror"
#if 0
        // серьёзная оптимизация
        // (i) ~10% прирост только с включением опций ниже.
        // @todo optimize Тонкая оптимизация OpenCL.
        //       http://khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html
        << " -cl-fast-relaxed-math"
        << " -cl-mad-enable"
#endif
#if 0
        // внимательная отладка
        // (i) Включать следует только при вылавливании блох: более чем
        //     20-кратное замедление 
        << " -cl-opt-disable"
#endif
        << "";

    return options.str();
}





inline void DungeonCrawl::fnErrorCL( int exitCode ) {
    std::cerr << "Код ошибки OpenCL: " << exitCode << std::endl;

    // @todo fine Выбрасывать исключение.
    std::cin.ignore();
    std::exit( exitCode );
}



} // porte
