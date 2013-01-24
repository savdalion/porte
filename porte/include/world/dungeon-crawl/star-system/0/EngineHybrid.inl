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

    aboutStarSystemCL( nullptr ),
    asteroidCL( nullptr ),
    planetCL( nullptr ),
    starCL( nullptr ),

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
    // освобождаем память, выделенную под параметры ядер
    if ( aboutStarSystemCL ) { clReleaseMemObject( aboutStarSystemCL ); }
    if ( asteroidCL ) { clReleaseMemObject( asteroidCL ); }
    if ( planetCL ) { clReleaseMemObject( planetCL ); }
    if ( starCL ) { clReleaseMemObject( starCL ); };

    // удаляем собранные ядра
    for (auto itr = kernelCL.begin(); itr != kernelCL.end(); ++itr) {
        errorCL |= clReleaseKernel( itr->second );
    }
#ifdef _DEBUG
    oclCheckError( errorCL, CL_SUCCESS );
#endif

    // при подготовке ядер создали список устройств, освобождаем
    free( devicesCL );

    // освобождаем очередь команд и контекст
    clReleaseCommandQueue( commandQueueCL );
    clReleaseContext( gpuContextCL );
}




void EngineHybrid::incarnate(
    std::shared_ptr< portulan_t >  p,
    real_t extentPortulan
) {
    EngineWithoutBooster::incarnate( p, extentPortulan );


    // Подготавливаем память под параметры для ядер OpenCL
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    if ( aboutStarSystemCL ) { clReleaseMemObject( aboutStarSystemCL ); }
    aboutStarSystemCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeStarSystem,
        // #! Если память выделена динамически, обращаемся к содержанию.
        &topology.aboutStarSystem,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    if ( asteroidCL ) { clReleaseMemObject( asteroidCL ); }
    asteroidCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeAsteroid,
        // #! Если память выделена динамически, обращаемся к содержанию.
        asteroid,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    if ( planetCL ) { clReleaseMemObject( planetCL ); }
    planetCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizePlanet,
        // #! Если память выделена динамически, обращаемся к содержанию.
        planet,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    if ( starCL ) { clReleaseMemObject( starCL ); };
    starCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStar,
        // #! Если память выделена динамически, обращаемся к содержанию.
        star,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );    


    // # У всех ядер часть набора параметров - одинакова. Воспользуемся.
    for (auto itr = kernelCL.cbegin(); itr != kernelCL.cend(); ++itr) {
        const cl_kernel kernel = itr->second;

        errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &aboutStarSystemCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 1, sizeof( const cl_mem ), &asteroidCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 2, sizeof( const cl_mem ), &planetCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 3, sizeof( const cl_mem ), &starCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        errorCL = clSetKernelArg( kernel, 4, sizeof( mTimestep ), &mTimestep );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // # Остальные параметры - отличаются или меняются каждый пульс.

    } // for (auto itr = kernelCL.cbegin() ...

}




inline void EngineHybrid::pulse( int n ) {

    assert( (n > 0)
        && "Умеем работать только с натуральным кол-вом пульсов." );

    // рассчитываем воздействия на тела звёздной системы
    // # Все элементы звёздной системы работают по единой схеме.
    // # Каждый элемент звёздной системы генерирует свой набор событий.
    // # Испускаемые элементом события хранятся в памяти событий у самого
    //   элемента.
    // # Событие может изменять неск. элементов. Поэтому, после "испускания"
    //   события каждый элемент просматривает др. элементы на наличие событий,
    //   которые могут оказать влияние на сам элемент.
    // # Чтобы избежать дублирования симметричных событий (например,
    //   столкновений), принимаем:
    //   Симметр. общее событие регистрируется *только* первым по алфавиту
    //   элементом. остальные участники просматривают такие события и создают
    //   на их основе конкретные (свои) события. Побочный эффект: меньше
    //   загрузка памяти общих событий, ускорение обработки.

    // выполняем 'n' циклов
    emitEvent( n );


    // @test
#if 1
{
    const auto tss = sizeof( pns::aboutStarSystem_t );
    const auto ta = sizeof( pns::aboutAsteroid_t );
    const auto tp = sizeof( pns::aboutPlanet_t );
    const auto ts = sizeof( pns::aboutStar_t );

    const auto tca = sizeof( pns::characteristicAsteroid_t );
    const auto ts3 = sizeof( pns::small3d_t );
    const auto tb3 = sizeof( pns::big3d_t );
    const auto tr1 = sizeof( pns::real_t );
    const auto tr2 = sizeof( pns::real2_t );
    const auto tr4 = sizeof( pns::real4_t );
    const auto tb = sizeof( bool );

    const auto te2 = sizeof( pns::eventTwo_t );
    const auto tpe = sizeof( pns::pointerElement_t );
    const auto te = sizeof( enum pns::EVENT );

    const auto tmm = sizeof( pns::memoryModel_t );
    const auto tm = sizeof( pns::model_t );
    const auto tfmm = sizeof( pns::frequencyMemoryModel_t );
    const auto tfm = sizeof( pns::frequencyModel_t );

    const bool test = true;
}
#endif


    // собираем статистику для элементов портулана
    statistics();
}




inline void EngineHybrid::emitEvent( int n ) {

    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // выполним 'n' пульсов
    // # Параметры для ядер уже подготовлены в incarnate().
    for (int p = 0; p < n; ++p) {

    // # Перед началом каждого пульса и в конце него элементы -
    //   упорядочены (оптимизированы). См. старания ниже.

    const cl_long pulselive = mLive.pulselive();

    // Подготавливаем элементы к созданию событий
#if 1
    {
        runCLKernel< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/begin" );
        runCLKernel< pns::STAR_COUNT >( "set/emit-event/star/begin" );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // Излучаем свои события
#if 1
    {
        // модель поведения всех элементов
        runCLKernel< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/direct" );
        //runCLKernel< pns::STAR_COUNT >( "set/emit-event/star/direct" );

        // модели поведения конкретных элементов
        // # Запускаются для всей группы элементов, т.к. в общем случае
        //   отрабатывать подобную модель могут много элементов.
        // @todo optimize fine Не запускать ядро модели, если ни один элемент
        //       не собирается отрабатывать эту модель.
        /*
        {
            const std::string key =
                "set/unique-emit-event/asteroid/grow-up/direct";
            const cl_kernel kernel = kernelCL[ key ];
            errorCL = clSetKernelArg( kernel, 5, sizeof( pulselive ), &pulselive );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
            runCLKernel< pns::ASTEROID_COUNT >( key );
            // @todo ...
        }
        */

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // Излучаем свои зависимые события
#if 1
    {
        runCLKernel< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/relative" );
        runCLKernel< pns::STAR_COUNT >( "set/emit-event/star/relative" );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // Уведомляем слушателей о своих событиях в звёздной системе
    // Тут же собираем информацию о необходимости оптимизации списков
#if 1
    // события астероидов
#if 1
    bool needOptimizeAsteroid = false;
    // обновляем данные на хосте
    {
        errorCL = clEnqueueReadBuffer(
            commandQueueCL,
            asteroidCL,
            CL_TRUE,
            0,
            memsizeAsteroid,
            asteroid,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }

    // просматриваем события
    for (size_t i = 0;
        // # Здесь можем воспользоваться признаком "Пустой элемент -
        //   дальше нет событий", т.к. элементы в начале каждого
        //   пульса - упорядочены. См. соглашение в начале пульса.
        (i < pns::ASTEROID_COUNT) && pns::presentAsteroid( &asteroid[ i ] );
        ++i
    ) {
        const auto& element = asteroid[ i ];
        const pns::emitterEvent_t& ee = element.emitterEvent;
        for (int w = ee.waldo - 1; w >= 0; --w) {
            const pns::eventTwo_t& e = ee.content[ w ];
            const pns::pointerElement_t piA = {
                pns::GE_ASTEROID,  i,  element.uid
            };

            // уведомляем подписчиков
            for (auto etr = StoreListenerAsteroid::begin();
                etr;  etr = StoreListenerAsteroid::next()
            ) { if ( etr ) {
                const auto& l = etr->listener.lock();
                l->beforeEvent( piA, e );
                // @todo Расписать события через 'switch', как в ядрах OpenCL.
            } }

            // смотрим, потребуются ли после этого события оптимизации
            if (e.uid == pns::E_DESTROY) {
                needOptimizeAsteroid = true;
            }

        } // for (int w = ...

    } // for (size_t i = ...
#endif

    // события планет
#if 1
    bool needOptimizePlanet = false;
    // @todo ...
#endif

    // события звёзд
#if 1
    bool needOptimizeStar = false;
    // обновляем данные на хосте
    {
        errorCL = clEnqueueReadBuffer(
            commandQueueCL,
            starCL,
            CL_TRUE,
            0,
            memsizeStar,
            star,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }

    // просматриваем события
    for (size_t i = 0;
        // # Здесь можем воспользоваться признаком "Пустой элемент -
        //   дальше нет событий", т.к. элементы в начале каждого
        //   пульса - упорядочены. См. соглашение в начале пульса.
        (i < pns::STAR_COUNT) && pns::presentStar( &star[ i ] );
        ++i
    ) {
        const auto& element = star[ i ];
        const pns::emitterEvent_t& ee = element.emitterEvent;
        for (int w = ee.waldo - 1; w >= 0; --w) {
            const pns::eventTwo_t& e = ee.content[ w ];
            const pns::pointerElement_t piA = {
                pns::GE_STAR,  i,  element.uid
            };

            // уведомляем подписчиков
            for (auto etr = StoreListenerStar::begin();
                etr;  etr = StoreListenerStar::next()
            ) { if ( etr ) {
                const auto& l = etr->listener.lock();
                l->beforeEvent( piA, e );
                // @todo Расписать события через 'switch', как в ядрах OpenCL.
            } }

            // смотрим, потребуются ли после этого события оптимизации
            if (e.uid == pns::E_DESTROY) {
                needOptimizeStar = true;
            }

        } // for (int w = ...

    } // for (size_t i = ...
#endif

#endif


    // Фиксируем характеристики согласно своим событиям
#if 1
    {
        runCLKernel< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/fix" );
        runCLKernel< pns::STAR_COUNT >( "set/emit-event/star/fix" );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // # Ядра используют признак "Встретился пустой элемент - дальше
    //   можно не просматривать". Поэтому из-за событий, меняющих признак
    //   "Элемент отсутствует" - см. pns::absent*() - требуется оптимизировать
    //   список элементов для след. пульса.
    // # Оптимизация должна быть после отработки событий, т.к. ядра могут
    //   использовать i-индексы для доступа к дополнительной информации об
    //   элементе. @todo ? Отказ от этого требования - возможность лишний раз
    //   не обновлять информацию на хосте.
    // оптимизация - ресурсоёмкая операция, стараемся не использовать
#if 1

    // оптимизация списка астероидов
#if 1
    // @todo optimize? fine? Оптимизировать в ядре OpenCL.
    if ( needOptimizeAsteroid ) {
        // обновим данные на хосте
        {
            errorCL = clEnqueueReadBuffer(
                commandQueueCL,
                asteroidCL,
                CL_TRUE,
                0,
                memsizeAsteroid,
                asteroid,
                0, nullptr, nullptr
            );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        }

        // оптимизируем
        pns::optimizeCountAsteroid( asteroid );

        // зальём данные на устройство
        {
            errorCL = clEnqueueWriteBuffer(
                commandQueueCL,
                asteroidCL,
                CL_TRUE,
                0,
                memsizeAsteroid,
                asteroid,
                0, nullptr, nullptr
            );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        }
    }
#endif

    // оптимизация списка планет
#if 1
    // @todo optimize fine Оптимизировать в ядре OpenCL.
    if ( needOptimizePlanet ) {
        // обновим данные на хосте
        {
            errorCL = clEnqueueReadBuffer(
                commandQueueCL,
                planetCL,
                CL_TRUE,
                0,
                memsizePlanet,
                planet,
                0, nullptr, nullptr
            );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        }

        // оптимизируем
        pns::optimizeCountPlanet( planet );

        // зальём данные на устройство
        {
            errorCL = clEnqueueWriteBuffer(
                commandQueueCL,
                planetCL,
                CL_TRUE,
                0,
                memsizePlanet,
                planet,
                0, nullptr, nullptr
            );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        }
    }
#endif

    // оптимизация списка звёзд
#if 1
    // @todo optimize fine Оптимизировать в ядре OpenCL.
    if ( needOptimizeStar ) {
        // обновим данные на хосте
        {
            errorCL = clEnqueueReadBuffer(
                commandQueueCL,
                starCL,
                CL_TRUE,
                0,
                memsizeStar,
                star,
                0, nullptr, nullptr
            );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        }

        // оптимизируем
        pns::optimizeCountStar( star );

        // зальём данные на устройство
        {
            errorCL = clEnqueueWriteBuffer(
                commandQueueCL,
                starCL,
                CL_TRUE,
                0,
                memsizeStar,
                star,
                0, nullptr, nullptr
            );
            oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        }
    }
#endif

#endif

    // мир становится старше
    mLive.inc( mTimestep );

    } // for (int p = 0; p < n; ++p)


    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        asteroidCL,
        CL_TRUE,
        0,
        memsizeAsteroid,
        asteroid,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        planetCL,
        CL_TRUE,
        0,
        memsizePlanet,
        planet,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        starCL,
        CL_TRUE,
        0,
        memsizeStar,
        star,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
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
    const cl_context_properties props[] =
        { CL_CONTEXT_PLATFORM, (cl_context_properties)platformCL, 0 };
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
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/set/helper.hcl" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/structure.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/asteroid.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/planet.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/star.h" )
        // все h-файлы выше включены, т.к. они включены в
        // star-system.h (используется во всех ядрах)
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/star-system.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/compute-utils.h" )
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




template< size_t GLOBAL_SIZE >
inline void EngineHybrid::runCLKernel( const std::string& key ) {
    assert( (kernelCL.find( key ) != kernelCL.cend())
        && "Ядро OpenCL не найдено." );

    static const size_t DIM = 1;
    static const size_t GS[] = { GLOBAL_SIZE };

    const cl_kernel kernel = kernelCL[ key ];
    const auto errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        DIM,
        nullptr,
        GS,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}




inline std::string EngineHybrid::commonConstantCLKernel() {

    std::ostringstream options;
    options
        // лечим точность для float
        << std::fixed

        << " -D __DEBUG"
        //<< " -D GPU_OPENCL"

        << " -D ASTEROID_COUNT=" << pns::ASTEROID_COUNT
        << " -D PLANET_COUNT=" << pns::PLANET_COUNT
        << " -D STAR_COUNT=" << pns::STAR_COUNT
        << " -D EMITTER_EVENT_COUNT=" << pns::EMITTER_EVENT_COUNT
        << " -D FEATURE_EVENT_COUNT=" << pns::FEATURE_EVENT_COUNT
        << " -D MEMORY_MODEL_COUNT=" << pns::MEMORY_MODEL_COUNT
        << " -D FREQUENCY_MEMORY_MODEL_COUNT=" << pns::FREQUENCY_MEMORY_MODEL_COUNT

        << std::scientific
        << " -D BIG_VALUE_BASE_0=" << pns::BIG_VALUE_BASE_0
        << " -D BIG_VALUE_BASE_1=" << pns::BIG_VALUE_BASE_1
        << " -D BIG_VALUE_BASE_2=" << pns::BIG_VALUE_BASE_2
        << " -D BIG_VALUE_BASE_3=" << pns::BIG_VALUE_BASE_3
        << " -D REAL_MAX=" << pns::REAL_MAX


        // точность сравнения значений с плав. точкой
        << " -D PRECISION=" << typelib::PRECISION

        // физические и геометрические константы
        << " -D PI=" << typelib::constant::pi
        << " -D SPEED_LIGHT=" << typelib::constant::physics::c
        << " -D CK_TEMPERATURE=" << typelib::constant::physics::CK
        << " -D G=" << typelib::constant::physics::G
        << " -D STEFAN_BOLTZMANN=" << typelib::constant::physics::stefanBoltzmann

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
        // #i ~10% прирост только с включением опций ниже.
        // @todo optimize Тонкая оптимизация OpenCL.
        //       http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
        << " -cl-fast-relaxed-math"
        << " -cl-mad-enable"
#endif
#if 0
        // внимательная отладка
        // #i Включать только при вылавливании блох: более чем
        //    20-кратное замедление.
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
