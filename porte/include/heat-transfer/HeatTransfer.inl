namespace porte {


template< size_t SX, size_t SY, size_t SZ >
inline HeatTransfer< SX, SY, SZ >::HeatTransfer(
    portulan_t* p
) :
    Engine( p ),
    gpuContextCL( nullptr ),
    commandQueueCL( nullptr ),
    temperatureCL( nullptr )
{
    // подготавливаем ядра OpenCL (ядра требуют компиляции)
    prepareCLKernel();
}





template< size_t SX, size_t SY, size_t SZ >
inline HeatTransfer< SX, SY, SZ >::~HeatTransfer() {

    /* - Нет: отдаём для OpenCL ссылки на созданные извне структуры...
    // освобождаем структуры для обмена данными с OpenCL
    clReleaseMemObject( temperatureCL );
    */
    // ...но временные структуры - исключение
    clReleaseMemObject( workTemperatureCL );

    // удаляем собранные ядра
    cl_int errorCL = CL_SUCCESS;
    for (auto itr = kernelCL.begin(); itr != kernelCL.end(); ++itr) {
        errorCL |= clReleaseKernel( itr->second );
    }
    //errorCL |= clReleaseProgram( programCL );
#ifdef _DEBUG
    oclCheckError( errorCL, CL_SUCCESS );
#endif

    // освобождаем очередь команд и контекст
    clReleaseCommandQueue( commandQueueCL );
    clReleaseContext( gpuContextCL );
}







template< size_t SX, size_t SY, size_t SZ >
inline void HeatTransfer< SX, SY, SZ >::operator()() {
    // (!) Карта уже должна быть синхронизирована с бустер-структурой.
    // (!) Структуры для передачи OpenCL должны быть подготовлены в prepareCLKernel().

    cl_int errorCL = CL_SUCCESS;

    // Размерность сетки
    const size_t GRID_WORK_DIM = 3;
    // Количество Work Item
    const size_t GRID_GLOBAL_WORK_SIZE[] = { SX, SY, SZ };
    // Размер Work Item
    /* - пусть OpenCL выберет лучшую сам
    const cl::NDRange GRID_LOCAL_WORK_COUNT(
        mSizeWorld.get<0>() / mSizeArea.get<0>(),  // = 3
        mSizeWorld.get<1>() / mSizeArea.get<1>(),  // = 3
        mSizeWorld.get<2>() / mSizeArea.get<2>()   // = 3
    );
    */
    // ячеек в рабочей группе = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT

    const cl_kernel kernel = kernelCL[ "calcHeatTransfer" ];

    // @test
    const size_t sizeT = sizeof( cl_mem );
    const size_t sizeW = sizeof( cl_mem );

    // подготавливаем...
    errorCL = clSetKernelArg( kernel, 0, sizeof( cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 1, sizeof( cl_mem ), &workTemperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    // @test
    const size_t nt = std::end( mPortulanBooster.booster().temperature ) -
                      std::begin( mPortulanBooster.booster().temperature );
    const size_t nte = sizeof( mPortulanBooster.booster().temperature );
    std::cout << "Общая температура \"до\": "
        << std::accumulate(
            std::begin( mPortulanBooster.booster().temperature ),
            std::end( mPortulanBooster.booster().temperature ),
            0.0f
        ) << std::endl;
#endif

    // отправляем...
    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        // dim
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // дожидаемся...
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    

    // получаем...
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        temperatureCL,
        CL_TRUE,
        0,
        sizeof( cl_float ) * pb_t::TEMPERATURE_SIZE,
        mPortulanBooster.booster().temperature,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    // @test
    std::cout << "Общая температура \"после\": "
        << std::accumulate(
            std::begin( mPortulanBooster.booster().temperature ),
            std::end( mPortulanBooster.booster().temperature ),
            0.0f
        ) << std::endl;
#endif

}








#if 0
// - Переписано через oclUtils. См. ниже.

// @source Проект "v3d".
template< size_t SX, size_t SY, size_t SZ >
inline void HeatTransfer< SX, SY, SZ >::prepareCLKernel() {

    // @source http://www10.informatik.uni-erlangen.de/Teaching/Courses/SS2010/SiWiR2/teaching/siwir2-lecture07-4on1.pdf
    // @source http://developer.amd.com/gpu_assets/OpenCL_Parallel_Computing_for_CPUs_and_GPUs_201003.pdf

    namespace fs = boost::filesystem;

    typedef portulan::Portulan3D< SX, SY, SZ >::numberLayer_t  nl_t;

    // Размерность сетки
    const size_t GRID_WORK_DIM = 3;
    // Количество Work Item
    const size_t GRID_GLOBAL_WORK_SIZE[] = { SX, SY, SZ };
    // Размер Work Item
    /* - пусть OpenCL выберет лучшую сам
    const cl::NDRange GRID_LOCAL_WORK_COUNT(
        mSizeWorld.get<0>() / mSizeArea.get<0>(),  // = 3
        mSizeWorld.get<1>() / mSizeArea.get<1>(),  // = 3
        mSizeWorld.get<2>() / mSizeArea.get<2>()   // = 3
    );
    */
    // ячеек в рабочей группе = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    // Подготавливаем ядра OpenCL
    /* - Не используем assert-проверку: вместо неё работаем через исключения.
         Причина: чище код.
    cl_int error = CL_SUCCESS;
    */
    cl::Program program;

    // Подготавливаем параметры для ядер
    std::ostringstream options;
    options
        // размер мира
        << " -D N=" << SX
        << " -D M=" << SY
        << " -D L=" << SZ
        // минимальная и максимальная координаты
        << " -D MINX=" << nl_t::minCoord().x
        << " -D MINY=" << nl_t::minCoord().y
        << " -D MINZ=" << nl_t::minCoord().z
        << " -D MAXX=" << nl_t::maxCoord().x
        << " -D MAXY=" << nl_t::maxCoord().y
        << " -D MAXZ=" << nl_t::maxCoord().z
        // константы для ускорения работы ядер
        << " -D NM="  << ( SX * SY )
        << " -D NML=" << ( SX * SY * SZ )

        // точность сравнения значений с плав. точкой
        //<< " -D PRECISION=" << typelib::PRECISION

        /* - @todo физические константы
        // перевод из Кельвина в Цельсий
        << " -D KELVIN_CELSIUS_TEMPERATURE=-273.15f"
        // Газовая постоянная, Дж / (Моль * K)
        // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
        << " -D R_GAS=8.31441f"
        */

        // путь к включаемым файлам (*.hcl)
        << " -I " << PATH_OPENCL
        // перфекционизм: предупреждения в ядрах считаем ошибками
        << " -Werror"

        // @todo optimize Включить оптимизацию OpenCL.
        //       "-cl-fast-relaxed-math", "-cl-mad-enable"
        //       и др.. - см. http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html
        << " -cl-opt-disable"

        << "";
    
#ifdef _DEBUG
    // @test
    std::cout << "OpenCL compile options:" << std::endl << options.str() << std::endl;
#endif

    try {
        std::vector< cl::Platform >  platforms;
        cl::Platform::get( &platforms );
        if (platforms.size() == 0) {
            std::cerr << std::endl << "Платформа не найдена." << std::endl;
            throw "Platform is not found.";
        }
 
        cl_context_properties properties[] =
           { CL_CONTEXT_PLATFORM, (cl_context_properties)( platforms[0] )(), 0 };
        contextCL = cl::Context( CL_DEVICE_TYPE_GPU, properties, nullptr, nullptr );
  
        deviceCL = contextCL.getInfo< CL_CONTEXT_DEVICES >();
        assert( !deviceCL.empty() && "Не найдено ни одного устройства с поддержкой OpenCL." );
  

        // Получаем исходный код программы
        assert( fs::exists( PATH_OPENCL ) && "Папка с исходным кодом OpenCL не существует." );
        assert( fs::is_directory( PATH_OPENCL ) && "Путь к файлам OpenCL не является файловой директорией." );

        cl::Program::Sources sources;

        // Работаем с одним источником, в который включены ядра движка
        const std::string f = PATH_OPENCL + "kernel.cl";
        assert( fs::exists( f ) && "Файл 'kernel.cl' не найден." );
        std::ifstream fstr( f );
        const std::string str = std::string(
            std::istreambuf_iterator< char >( fstr ),
            std::istreambuf_iterator< char >()
            // добавляем к файлу уникальный хвост, чтобы OpenCL не думал
            // брать построенную ранее программу из кеша устройства: иначе
            // рискуем получать феноменальные ошибки, когда изменения во
            // *включаемых файлах* учитываются "через раз"
        ) + "\n/*" + boost::lexical_cast< std::string >( boost::posix_time::second_clock::local_time() ) + "*/";
        sources.push_back( std::make_pair( str.c_str(), str.length() + 1 ) );


        // Собираем программу
        program = cl::Program( contextCL, sources );
        program.build( deviceCL, options.str().c_str() );
  
        // Компилируем ядра
        VECTOR_CLASS< cl::Kernel >  kernels;
        program.createKernels( &kernels );

        // Скомпилированные ядра собираем в карту для дальнейшего использования
        for (auto itr = kernels.cbegin(); itr != kernels.cend(); ++itr) {
            const std::string name =
                (std::string)itr->getInfo< CL_KERNEL_FUNCTION_NAME >();
            kernelCL[ name ] = *itr;
        }

        // Инициализируем очередь для команд
        queueCL = cl::CommandQueue( contextCL, deviceCL[0] );
  

        /* - @example Исполнение ядра с ожиданием результата.
        cl::Event event;
        //const cl::Kernel& kernel = kernelCL.cbegin()->second;
        const cl::Kernel& kernel = kernelCL[ "calcVisualHideMap" ];
        queueCL.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            GRID_GLOBAL_WORK_SIZE,
            // @todo optimize Передавать cl::NullRange - OpenCL сам попробует
            //       подобрать оптимальное кол-во рабочих групп.
            GRID_LOCAL_WORK_COUNT,
            nullptr,
            &event
        );
        event.wait();
        */

      } catch ( const cl::Error& ex ) {
          std::cerr << "(!) " << ex.what() << "(" << ex.err() << ")" << std::endl;
          for (auto itr = deviceCL.cbegin(); itr != deviceCL.cend(); ++itr) {
              std::cerr << program.getBuildInfo< CL_PROGRAM_BUILD_LOG >( *itr ) << std::endl;
          }
          throw "OpenCL kernels are not created.";
      }
 
      assert( !kernelCL.empty()
          && "Результатом выполнения метода должны быть собранные вычислительные ядра OpenCL." );
}
#endif







template< size_t SX, size_t SY, size_t SZ >
inline void HeatTransfer< SX, SY, SZ >::prepareCLKernel() {

    // Подготавливаем ядра OpenCL
    // @source http://nvidia.com/content/cuda/cuda-downloads.html / oclMarchingCubes.cpp

    cl_int errorCL = CL_SUCCESS;
    cl_device_id* devicesCL = nullptr;
    cl_uint deviceUsedCL = 0;
    cl_uint devCountCL = 0;
    cl_platform_id platformCL = nullptr;
    //cl_context gpuContextCL = nullptr;
    //cl_command_queue commandQueueCL = nullptr;


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
    gpuContextCL = clCreateContext( props, 1, &devicesCL[ deviceUsedCL ], nullptr, nullptr, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "Выбрано устройство (OpenCL):" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif


    // create a command-queue
    commandQueueCL = clCreateCommandQueue( gpuContextCL, devicesCL[ deviceUsedCL ], 0, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // Компилируем вычислительные ядра

    const std::vector< std::string > kernelNames = boost::assign::list_of
        ( "calcHeatTransfer" )
    ;
    const std::string searchPath = PATH_CL + "/heat-transfer";
    for (auto itr = std::begin( kernelNames ); itr != std::end( kernelNames ); ++itr) {
        const std::string kernelName = *itr;
        // Program Setup
        size_t programLength;
        const std::string fileKernel = kernelName + ".cl";
        /* - Проще. См. ниже.
        char* pathAndNameCL = shrFindFilePath( fileKernel.c_str(),  searchPath.c_str() );
        oclCheckErrorEX( (pathAndNameCL != nullptr), true, &fnErrorCL );
        */
        const std::string pathAndName = searchPath + "/" + fileKernel;
        char* sourceCL = oclLoadProgSource( pathAndName.c_str(), "", &programLength );
        oclCheckErrorEX( (sourceCL != nullptr), true, &fnErrorCL );

        // create the program
        cl_program programCL = clCreateProgramWithSource(
            gpuContextCL,  1,  (const char**)&sourceCL,  &programLength, &errorCL
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    

        // Подготавливаем параметры для ядер
        typedef portulan::Portulan3D< SX, SY, SZ >::numberLayer_t  nl_t;
        std::ostringstream options;
        options
            // размер мира
            << " -D N=" << SX
            << " -D M=" << SY
            << " -D L=" << SZ
            // минимальная и максимальная координаты
            << " -D MINX=" << nl_t::minCoord().x
            << " -D MINY=" << nl_t::minCoord().y
            << " -D MINZ=" << nl_t::minCoord().z
            << " -D MAXX=" << nl_t::maxCoord().x
            << " -D MAXY=" << nl_t::maxCoord().y
            << " -D MAXZ=" << nl_t::maxCoord().z
            // константы для ускорения работы ядер
            << " -D NM="  << ( SX * SY )
            << " -D NML=" << ( SX * SY * SZ )

            // точность сравнения значений с плав. точкой
            << " -D PRECISION=" << typelib::PRECISION

            /* - @todo физические константы
            // перевод из Кельвина в Цельсий
            << " -D KELVIN_CELSIUS_TEMPERATURE=-273.15f"
            // Газовая постоянная, Дж / (Моль * K)
            // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
            << " -D R_GAS=8.31441f"
            */

            // путь к включаемым файлам (*.hcl)
            << " -I " << searchPath
            // перфекционизм: предупреждения в ядрах считаем ошибками
            << " -Werror"

            // @todo optimize Включить оптимизацию OpenCL.
            //       "-cl-fast-relaxed-math", "-cl-mad-enable"
            //       и др.. - см. http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html
            << " -cl-opt-disable"

            << "";
    
    #ifdef _DEBUG
        // @test
        std::cout << "OpenCL compile options:" << std::endl << options.str() << std::endl;
    #endif

        // build the program
        errorCL = clBuildProgram( programCL, 0, nullptr, options.str().c_str(), nullptr, nullptr );
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
        kernelCL[ kernelName ] = oneKernelCL;

        // Некоторые ресурсы надо освободить прямо сейчас
        clReleaseProgram( programCL );

    } // for (auto itr



    // Подготавливаем структуры для обмена с OpenCL
    temperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        sizeof( cl_float ) * pb_t::TEMPERATURE_SIZE,
        mPortulanBooster.booster().temperature,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    workTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // выделяем память под временную структуру
        CL_MEM_READ_WRITE,
        sizeof( cl_float ) * pb_t::TEMPERATURE_SIZE,
        nullptr,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}






template< size_t SX, size_t SY, size_t SZ >
inline void HeatTransfer< SX, SY, SZ >::fnErrorCL( int exitCode ) {
    std::cerr << "Инициализация OpenCL не завершена. Код ошибки: " << exitCode << std::endl;

    // @todo fine Выбрасывать исключение.
    std::cin.ignore();
    std::exit( exitCode );
}



} // porte
