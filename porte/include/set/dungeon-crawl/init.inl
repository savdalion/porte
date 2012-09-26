namespace porte {


/**
* Часть методов класса DungeonCrawl.
*/


inline void DungeonCrawl::init() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    initComponent();
#endif

#ifdef LIVING_DUNGEONCRAWL_PORTE
    initLiving();
#endif

#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
    initTemperature();
#endif
}





#ifdef COMPONENT_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initComponent() {
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareComponentCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pc = portulan::planet::set::dungeoncrawl::component;

#ifdef _DEBUG
    std::cout << "Определяем состав планеты ..";
#endif

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

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initLiving() {
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareComponentCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pl = portulan::planet::set::dungeoncrawl::living;

#ifdef _DEBUG
    std::cout << "Населяем планету ..";
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
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initTemperature() {
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareTemperatureCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pt = portulan::planet::set::dungeoncrawl::temperature;

#ifdef _DEBUG
    std::cout << "Создаём погоду ..";
#endif

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

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif


} // porte
