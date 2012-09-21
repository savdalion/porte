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
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
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




inline void DungeonCrawl::init() {
    initComponent();
    initLiving();
    initTemperature();
}




inline void DungeonCrawl::initComponent() {
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareComponentCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pc = portulan::planet::set::dungeoncrawl::component;

    static const size_t grid = pd::COMPONENT_GRID;

    // размерность сетки
    static const size_t GRID_WORK_DIM = 3;
    // количество Work Item
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };
    // размер Work Item
    /* - Пусть OpenCL выберет лучший размер сам.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( ... );
    */
    // ячеек в рабочей группе = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    // Инициализируем сетку
    const cl_kernel kernel = kernelCL[ "scale/component/top/init" ];

    // 0
    errorCL = clSetKernelArg( kernel, 0, sizeof( cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 1
    errorCL = clSetKernelArg( kernel, 1, sizeof( cl_mem ), &componentCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        componentCL,
        CL_TRUE,
        0,
        memsizeComponent,
        mPortulan->topology().topology().component.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

}




inline void DungeonCrawl::initLiving() {
    // @todo ...
}




inline void DungeonCrawl::initTemperature() {
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareTemperatureCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pt = portulan::planet::set::dungeoncrawl::temperature;

    static const size_t grid = pd::TEMPERATURE_GRID;

    // размерность сетки
    static const size_t GRID_WORK_DIM = 3;
    // количество Work Item
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };
    // размер Work Item
    /* - Пусть OpenCL выберет лучший размер сам.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( ... );
    */
    // ячеек в рабочей группе = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    // Инициализируем сетку
    const cl_kernel kernel = kernelCL[ "scale/temperature/top/init" ];


    /* @test
    pd::test_t test;
    const size_t memsizeTest = sizeof( pd::test_t );
    cl_mem testCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeTest,
        &test,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    errorCL = clSetKernelArg( kernel, 0, sizeof( cl_mem ), &testCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    */


    // 0
    errorCL = clSetKernelArg( kernel, 0, sizeof( cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 1
    errorCL = clSetKernelArg( kernel, 1, sizeof( cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        temperatureCL,
        CL_TRUE,
        0,
        memsizeTemperature,
        mPortulan->topology().topology().temperature.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

}






inline void DungeonCrawl::pulse( int n ) {
#if 0
    // (!) Карта уже должна быть синхронизирована с бустер-структурой.
    // (!) Структуры для передачи OpenCL должны быть подготовлены в prepareCLKernel().

    cl_int errorCL = CL_SUCCESS;

    // размерность сетки
    const size_t GRID_WORK_DIM = 3;
    // количество Work Item
    const size_t GRID_GLOBAL_WORK_SIZE[] = { SX, SY, SZ };
    // размер Work Item
    /* - Пусть OpenCL выберет лучший размер сам.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( 3, 3, 3 );
    */
    // ячеек в рабочей группе = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT

#ifdef _DEBUG
    // @test
    const size_t sizeT = sizeof( cl_mem );
    const size_t sizeW = sizeof( cl_mem );
#endif

    // подготавливаем...

#ifdef _DEBUG
    /* @test
    const size_t nb = std::end( mPortulanBooster.booster().temperature ) -
                      std::begin( mPortulanBooster.booster().temperature );
    const size_t sbe = sizeof( mPortulanBooster.booster() );
    std::cout << "Общая температура \"до\": "
        << std::accumulate(
            std::begin( mPortulanBooster.booster().temperature ),
            std::end( mPortulanBooster.booster().temperature ),
            0.0f
        ) << std::endl;
    */
#endif


    // реализуем требуемое кол-во пульсов
    for (int q = 0; q < n; ++q) {

        // отправляем...

        // 1. Посчитаем среднюю температуру, запишем во временный объём.
        const cl_kernel kernelCalcUniformHeatTransfer = kernelCL[ "calcUniformHeatTransfer" ];

        errorCL = clSetKernelArg( kernelCalcUniformHeatTransfer, 0, sizeof( cl_mem ), &boosterCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernelCalcUniformHeatTransfer, 1, sizeof( cl_mem ), &workBoosterCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clEnqueueNDRangeKernel(
            commandQueueCL,
            kernelCalcUniformHeatTransfer,
            GRID_WORK_DIM,
            nullptr,
            GRID_GLOBAL_WORK_SIZE,
            nullptr,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


        // 2. Перенесём результат из временной таблицы в таблицу температур.
        const cl_kernel kernelFixResult = kernelCL[ "fixResult" ];

        errorCL = clSetKernelArg( kernelFixResult, 0, sizeof( cl_mem ), &boosterCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernelFixResult, 1, sizeof( cl_mem ), &workBoosterCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clEnqueueNDRangeKernel(
            commandQueueCL,
            kernelFixResult,
            GRID_WORK_DIM,
            nullptr,
            GRID_GLOBAL_WORK_SIZE,
            nullptr,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    
    } // for (int q = 0; q < n; ++q)


    // получаем результат после "n" пульсов...
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        boosterCL,
        CL_TRUE,
        0,
        sizeof( mPortulanBooster.booster() ),
        mPortulanBooster.booster().temperature,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    /* @test
    std::cout << "Общая температура \"после\": "
        << std::accumulate(
            std::begin( mPortulanBooster.booster().temperature ),
            std::end( mPortulanBooster.booster().temperature ),
            0.0f
        ) << std::endl;
    */
#endif

#endif
}







#if 0
// - Переписано через oclUtils. См. ниже.

// @source Проект "v3d".
template< size_t SX, size_t SY, size_t SZ >
inline void UniformHeatTransfer< SX, SY, SZ >::prepareCLKernel() {

    // @source http://www10.informatik.uni-erlangen.de/Teaching/Courses/SS2010/SiWiR2/teaching/siwir2-lecture07-4on1.pdf
    // @source http://developer.amd.com/gpu_assets/OpenCL_Parallel_Computing_for_CPUs_and_GPUs_201003.pdf

    namespace fs = boost::filesystem;

    typedef portulan::Portulan< SX, SY, SZ >::numberLayer_t  nl_t;

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





inline void DungeonCrawl::prepareCLKernel() {
    prepareComponentCLKernel();
    prepareLivingCLKernel();
    prepareTemperatureCLKernel();
}





inline void DungeonCrawl::prepareComponentCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pc = portulan::planet::set::dungeoncrawl::component;

    // # Контекст и очередь команд инициализированы в конструкторе.

    // Компилируем ядра OpenCL

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;
    // @todo Искать по папкам в "scale". Сейчас - фиксированный путь, плохо.
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
    

        // Подготавливаем параметры для ядер
        static const size_t grid = pd::COMPONENT_GRID;
        typedef typelib::StaticMapContent3D< grid, grid, grid >  smc_t;

        // масштаб для рабочей сетки считаем здесь, чтобы не нагружать ядра
        // # Масштаб - сколько метров содержит 1 ячейка рабочей сетки.
        const float scale =
            mPortulan->topology().topology().aboutPlanet.size /
            static_cast< float >( grid );

        std::ostringstream options;
        options
            // лечим точность для float
            << std::fixed
            // #! Рабочие сетки декларируются отдельно: есть методы
            //   в helper.hcl, которые используют их.
            << " -D GRID=" << grid
            << " -D MIN_COORD_GRID=" << smc_t::minCoord().x
            << " -D MAX_COORD_GRID=" << smc_t::maxCoord().x
            << " -D SCALE=" << scale
            << commonConstantCLKernel()
            << commonOptionCLKernel()
            << "";
    
#ifdef _DEBUG
        // @test
        std::cout << "Опции OpenCL для ключа ядра \"" << kernelKey << "\":" << std::endl << options.str() << std::endl;
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
        kernelCL[ kernelKey ] = oneKernelCL;

        // Некоторые ресурсы надо освободить прямо сейчас
        clReleaseProgram( programCL );

    } // for (auto itr

}





inline void DungeonCrawl::prepareLivingCLKernel() {
    // @todo ...
}






inline void DungeonCrawl::prepareTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pt = portulan::planet::set::dungeoncrawl::temperature;

    // # Контекст и очередь команд инициализированы в конструкторе.

    // Компилируем ядра OpenCL

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/temperature/top/init" )
    ;
    // @todo Искать по папкам в "scale". Сейчас - фиксированный путь, плохо.
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
    

        // Подготавливаем параметры для ядер
        static const size_t grid = pd::TEMPERATURE_GRID;
        typedef typelib::StaticMapContent3D< grid, grid, grid >  smc_t;

        // масштаб для рабочей сетки считаем здесь, чтобы не нагружать ядра
        // # Масштаб - сколько метров содержит 1 ячейка рабочей сетки.
        const float scale =
            mPortulan->topology().topology().aboutPlanet.size /
            static_cast< float >( grid );

        std::ostringstream options;
        options
            // лечим точность для float
            << std::fixed
            // #! Рабочие сетки декларируются отдельно: есть методы
            //   в helper.hcl, которые используют их.
            << " -D GRID=" << grid
            << " -D MIN_COORD_GRID=" << smc_t::minCoord().x
            << " -D MAX_COORD_GRID=" << smc_t::maxCoord().x
            << " -D SCALE=" << scale
            << commonConstantCLKernel()
            << commonOptionCLKernel()
            << "";
    
#ifdef _DEBUG
        // @test
        std::cout << "Опции OpenCL для ключа ядра \"" << kernelKey << "\":" << std::endl << options.str() << std::endl;
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

    static std::ostringstream options;
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
        << " -D LIFE_CYCLE=" << pd::LIFE_CYCLE
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

        // добавляем к настройкам (в 2012 г. драйвер OpenCL от NVIDIA
        // научился хешировать файл без учёта комментариев) уникальный
        // хвост, чтобы OpenCL не думал брать построенную ранее программу
        // из кеша устройства: иначе рискуем получать феноменальные ошибки,
        // когда изменения во *включаемых файлах* учитываются "через раз"
        << " -D BUILD_RANDSTAMP=" << randstamp

        << "";

    return options.str();
}





inline std::string DungeonCrawl::commonOptionCLKernel() {
    static std::ostringstream options;
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
