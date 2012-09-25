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
    randomSeed( 131071u ),

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




inline void DungeonCrawl::init() {
    //initComponent();
    initLiving();
    //initTemperature();
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
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareComponentCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pl = portulan::planet::set::dungeoncrawl::living;

#ifdef _DEBUG
    std::cout << "Населяем планету особями .";
#endif

    static const size_t grid = pd::LIVING_GRID;

    // размерность сетки
    static const size_t GRID_WORK_DIM = 3;
    // количество Work Item
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };
    // размер Work Item
    /* - Пусть OpenCL выберет лучший размер сам.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( ... );
    */
    // ячеек в рабочей группе = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    auto& tp = mPortulan->topology().topology();

    // Инициализируем сетку

    // очищаем матрицу количеств
    cl_kernel kernel = kernelCL[ "scale/living/top/clear" ];

    // 0
    errorCL = clSetKernelArg( kernel, 0, sizeof( cl_mem ), &livingCL );
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

    /** - Синхронизируем поток в начале цикла (см. ниже).
          Благодаря параллельной работе CPU и GPU это позволит
          сократить время на выполнение кода.
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    */

#ifdef _DEBUG
    std::cout << ".";
#endif


    // вызываем init() до тех пор, пока кол-во особей на планете
    // не будет близко к желаемому
    kernel = kernelCL[ "scale/living/top/init" ];

    // 0
    errorCL = clSetKernelArg( kernel, 0, sizeof( cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 1
    errorCL = clSetKernelArg( kernel, 1, sizeof( cl_mem ), &livingCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 2
    // На сколько мы приблизились к расселению заданного кол-ва особей.
    // подготавливаем проверочный аргумент
    pd::zoneLivingCountComplete_t zoneCountComplete;
    static const size_t memsizeZoneCountComplete =
        sizeof( pd::zoneOneLivingCountComplete_t ) *
        pd::LIVING_COUNT * pd::LIFE_CYCLE_COUNT;
    cl_mem zoneCountCompleteCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeZoneCountComplete,
        &zoneCountComplete,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 2, sizeof( cl_mem ), &zoneCountCompleteCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 3
    // # Заселять мир будем исключительно взрослыми особями.
    const pd::LIFE_CYCLE lifeCycle = pd::LC_ADULT;
    errorCL = clSetKernelArg( kernel, 3, sizeof( pd::LIFE_CYCLE ), &lifeCycle );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 4
    errorCL = clSetKernelArg( kernel, 4, sizeof( cl_mem ), &componentCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 5
    errorCL = clSetKernelArg( kernel, 5, sizeof( cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // 6
    // Нет смысла добавлять другое значение при каждом вызове ядра, чтобы
    // разнообразить мир: в ядре OpenCL предостаточно "магических чисел".
    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernel, 6, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    const auto& al = tp.aboutPlanet.living;

    // желаемое кол-во особей
    float allZoneDesireCount = 0.0f;
    pd::zoneLivingCount_t zoneDesireCount = {};
    /* - Проще копировать код. См. ниже.
    const auto fnDesireCount = [ &zoneDesireCount, lifeCycle ] (
        float& zolValue,
        const pl::zoneLiving_t& zl
    ) -> void {
        const pl::CODE_LIVING code = zl.code;
        if (code != pl::CL_NONE) {
            const float q = zl.count;
            zolValue += q;
            zoneDesireCount[ code ][ lifeCycle ].all += q;
        }
    };
    */
    for (size_t k = 0; k < pd::LIVING_CELL; ++k) {
        // space
        {
            const auto& zk = al.space[k];
            const pl::CODE_LIVING code = zk.code;
            if (code != pl::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].space += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // atmosphere
        {
            const auto& zk = al.atmosphere[k];
            const pl::CODE_LIVING code = zk.code;
            if (code != pl::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].atmosphere += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // crust
        {
            const auto& zk = al.crust[k];
            const pl::CODE_LIVING code = zk.code;
            if (code != pl::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].crust += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // mantle
        {
            const auto& zk = al.mantle[k];
            const pl::CODE_LIVING code = zk.code;
            if (code != pl::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].mantle += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // core
        {
            const auto& zk = al.core[k];
            const pl::CODE_LIVING code = zk.code;
            if (code != pl::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].core += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
    } // for (size_t k


    // вызываем init(), анализируем полученное кол-во
    // @see Параметры для init().
    size_t iteration = 0;
    for ( ; ; ) {
        // синхронизируем потоки
        // (см. выше прим. после вызова ядра clear())
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // результат
        // первый вызов - проинициализированное выше нулями содержание,
        // последующие - результат работы ядра init()
        // @todo optimize Попробовать использовать livingCL без
        //       закачки данных в tp.living.content.
        errorCL = clEnqueueReadBuffer(
            commandQueueCL,
            livingCL,
            CL_TRUE,
            0,
            memsizeLiving,
            tp.living.content,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // подсчитываем кол-ва
        float allZoneCount = 0.0f;
        pd::zoneLivingCount_t zoneCount = {};
        // пробежим по всем ячейкам
        static const size_t LG = pd::LIVING_GRID * pd::LIVING_GRID * pd::LIVING_GRID;
        for (size_t i = 0; i < LG; ++i) {
            // в каждой ячейке содержится не более LIVING_CELL особей
            for (size_t k = 0; k < pd::LIVING_CELL; ++k) {
                // информация об особях сгруппирована по LIFE_CYCLE
                for (size_t l = static_cast< size_t >( pd::LC_EMBRYO);
                     l < static_cast< size_t >( pd::LC_last );  ++l
                ) {
                    const pl::portionLiving_t& p = tp.living.content[i][k][l];
                    const pl::CODE_LIVING code = p.code;
                    assert( ((code >= pl::CL_NONE) && (code < pl::CL_last))
                        && "Код особи не распознан. Вероятно, ошибка при инициализации матрицы." );
                    // суммируем (инициализация была выше)
                    zoneCount[ code ][ l ].all += p.count;
                    allZoneCount += p.count;
                }
            }
        }

        // кол-во особей должно быть близко к желаемому
        std::memset( zoneCountComplete, false, memsizeZoneCountComplete );
        bool allComplete = true;
        for (size_t code = 0; code < pd::LIVING_COUNT; ++code) {
            for (size_t l = static_cast< size_t >( pd::LC_EMBRYO);
                    l < static_cast< size_t >( pd::LC_last );  ++l
            ) {
                const float desireCount = zoneDesireCount[ code ][ l ].all;
                const float count       = zoneCount[ code ][ l ].all;
                // # Не используем группировку по зонам.
                auto& zcc = zoneCountComplete[ code ][ l ];
                zcc.all = (desireCount <= count * 1.1f);
                if ( !zcc.all ) {
                    allComplete = false;
                    break;
                }
            }
            if ( !allComplete ) {
                // true уже не будет
                break;
            }

        } // for (size_t code

        if ( allComplete ) {
            // планета заселена, количества особей соотв. желаемым
            break;
        }

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
        /* - (синхронизируем в начале цикла)
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        */

        // результат
        // (получаем в начале цикла)

        ++iteration;

#ifdef _DEBUG
        //std::cout << ".";
        // человечный индикатор прогресса
        static const size_t W = 50;
        static const size_t PRECISION_DIGIT = 0;
        static const float PRECISION_POW =
            boost::math::pow< PRECISION_DIGIT >( 10 );
        if (iteration % W == 0) {
            const float percent = std::floor(
                allZoneCount / allZoneDesireCount * 100.0f * PRECISION_POW
            ) / PRECISION_POW;
            std::ostringstream ss;
            ss << std::fixed << std::setprecision( PRECISION_DIGIT ) <<
                " " << percent << " %";
            std::cout << ss.str() <<
                std::string( ss.str().length(), '\b');
        }
#endif
    } // for ( ; ; )

#ifdef _DEBUG
    std::cout << " ОК          " << std::endl;
#endif
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
    //prepareComponentCLKernel();
    prepareLivingCLKernel();
    //prepareTemperatureCLKernel();
}





inline void DungeonCrawl::prepareComponentCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;

    // Подготавливаем ядерные опции
    static const size_t grid = pd::COMPONENT_GRID;
    
    // Компилируем ядра OpenCL
    compileCLKernel< grid >( kernelKeys );
}





inline void DungeonCrawl::prepareLivingCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/living/top/clear" )
        ( "scale/living/top/init" )
    ;

    // Подготавливаем ядерные опции
    static const size_t grid = pd::LIVING_GRID;

    std::ostringstream options;
    options
        // лечим точность для float
        << std::fixed

        // Для метода init(), определяют как быстро будет
        // увеличиваться кол-во особей в 1-й ячейке.
        // Коэф. прямо пропорционален желаемому кол-ву из zoneLiving_t.
        // (малые значения - быстрее вычисления, но более грубое
        // расселение особей)
        << " -D ITERATION_GROW_COUNT=" << 10

        // При включении:
        // Наличие в соседних ячейках подобных особей повышает вероятность
        // увеличения кол-ва особей в текущей ячейке.
        // #! Т.к. расселение особей ведётся без промежуточной матрицы
        //    и в ядре OpenCL отсутствует возможность синхронизовать потоки
        //    на глобальном уровне, возможна неточность при подсчёте кол-ва
        //    соседей. Здесь ошибка в кол-ве - вполне приемлема. Но надо
        //    помнить, что в общем случае она влечёт разное наполнение
        //    матриц даже при одинаковом "seed" (см. __kernel).
        //<< " -D WITH_NEIGHBOUR_EXPANSION"

        // При включении:
        // Наличие в текущей ячейке подобных особей повышает вероятность
        // увеличения кол-ва особей в этой же ячейке.
        // Матрица, в отличии от WITH_NEIGHBOUR_EXPANSION, стабильна.
        << " -D WITH_SELF_EXPANSION"

        << "";

    // Компилируем ядра OpenCL
    compileCLKernel< grid >( kernelKeys, options.str() );
}






inline void DungeonCrawl::prepareTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/temperature/top/init" )
    ;

    // Подготавливаем ядерные опции
    static const size_t grid = pd::TEMPERATURE_GRID;

    // Компилируем ядра OpenCL
    compileCLKernel< grid >( kernelKeys );
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
