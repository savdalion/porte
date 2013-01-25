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

    errorCL( CL_SUCCESS )
{
    // Подготавливаем контекст и очередь команд для работы с OpenCL
    prepareCL();

    // Подготавливаем ядра OpenCL (требуют компиляции)
    prepare();
}




inline EngineHybrid::~EngineHybrid() {
    // обо всём позаботятся объекты из ::cl
}




void EngineHybrid::incarnate(
    std::shared_ptr< portulan_t >  p,
    real_t extentPortulan
) {
    // # 'p' обновляет 'mPortulan' у родителя.
    EngineWithoutBooster::incarnate( p, extentPortulan );


    // Подготавливаем память под параметры для ядер OpenCL
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    aboutStarSystemBCL = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeStarSystem,
        &topology.aboutStarSystem,
        &errorCL
    );

    asteroidBCL = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeAsteroid,
        asteroid,
        &errorCL
    );

    planetBCL = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizePlanet,
        planet,
        &errorCL
    );

    starBCL = cl::Buffer(
        mContextCL,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStar,
        star,
        &errorCL
    );


    // # У всех ядер часть набора параметров - одинакова. Воспользуемся.
    for (auto itr = std::begin( mKernelCL ); itr != std::end( mKernelCL ); ++itr) {
        cl::Kernel& kernel = itr->second;

        kernel.setArg( 0, aboutStarSystemBCL );
        kernel.setArg( 1, asteroidBCL );
        kernel.setArg( 2, planetBCL );
        kernel.setArg( 3, starBCL );
        kernel.setArg( 4, mTimestep );

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
    try {
        emitEvent( n );
    } catch ( cl::Error& ex ) {
        const std::string s =
            static_cast< std::string >( ex.what() ) +
            " (" + boost::lexical_cast< std::string >( ex.err() ) + ")";
        std::cerr << s << std::endl;
        throw Exception( s );
    }


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

    
        // сформируем согласованные очереди для OpenCL
        // # В блоках ниже исп. одинаковые порции событий для каждого
        //   элемента звёздной системы.
        //static const size_t PEN = 1;
        vectorEventCL_t  emitBegin(  2 );
        vectorEventCL_t  emitDirect( 1 );


        // Подготавливаем элементы к созданию событий
#if 1
    /*
        cl::Event asteroidBegin;
        enqueueEventKernelCL< pns::ASTEROID_COUNT >(
            "set/emit-event/asteroid/begin",  &asteroidBegin );
        cl::Event asteroidDirect;
        enqueueEventKernelCL< pns::ASTEROID_COUNT >(
            "set/emit-event/asteroid/direct", &asteroidDirect );

        cl::Event starBegin;
        enqueueEventKernelCL< pns::STAR_COUNT >(
            "set/emit-event/star/begin",      &starBegin );
        cl::Event starDirect;
        enqueueEventKernelCL< pns::STAR_COUNT >(
            "set/emit-event/star/direct",     &starDirect );
    */

        cl::Event asteroidBegin;
        //enqueueEventKernelCL< pns::ASTEROID_COUNT >(
        //    "set/emit-event/asteroid/begin",  &asteroidBegin );
        cl::Event starBegin;
        //enqueueEventKernelCL< pns::STAR_COUNT >(
        //    "set/emit-event/star/begin",      &starBegin );
        //mQueueCL.flush();

        cl::Event asteroidDirect;
        cl::Event starDirect;
        const vectorEventCL_t afterBegin //= boost::assign::list_of
            //( asteroidBegin )
            //( starBegin )
        ;
        enqueueEventKernelCL< pns::ASTEROID_COUNT >(
            "set/emit-event/asteroid/direct",  afterBegin,  &asteroidDirect );
        //enqueueEventKernelCL< pns::STAR_COUNT >(
        //    "set/emit-event/star/direct",      afterBegin,  &starDirect );

        mQueueCL.flush();
#endif


        // ожидаем завершения отработки очереди
        mQueueCL.finish();

    } // for (int p = 0 ...



#if 0
// @todo ...
    // выполним 'n' пульсов
    // # Параметры для ядер уже подготовлены в incarnate().
    for (int p = 0; p < n; ++p) {

    // # Перед началом каждого пульса и в конце него элементы -
    //   упорядочены (оптимизированы). См. старания ниже.

    const cl_long pulselive = mLive.pulselive();

    // Подготавливаем элементы к созданию событий
#if 1
    {
        enqueueKernelCL< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/begin" );
        enqueueKernelCL< pns::STAR_COUNT >( "set/emit-event/star/begin" );

        // синхронизация
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // Излучаем свои события
#if 1
    {
        // модель поведения всех элементов
        enqueueKernelCL< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/direct" );
        //enqueueKernelCL< pns::STAR_COUNT >( "set/emit-event/star/direct" );

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
            enqueueKernelCL< pns::ASTEROID_COUNT >( key );
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
        enqueueKernelCL< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/relative" );
        enqueueKernelCL< pns::STAR_COUNT >( "set/emit-event/star/relative" );

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
        enqueueKernelCL< pns::ASTEROID_COUNT >( "set/emit-event/asteroid/fix" );
        enqueueKernelCL< pns::STAR_COUNT >( "set/emit-event/star/fix" );

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

#endif
}




inline void EngineHybrid::statistics() {
    const auto& os = mPortulan.lock()->orderStatistics();
    for (auto itr = os.cbegin(); itr != os.cend(); ++itr) {
        itr->second->grabPulse( mLive.pulselive() );
    }
}




inline void EngineHybrid::prepareCL() {

    std::vector< cl::Platform >  mPlatformCL;
    cl::Platform::get( &mPlatformCL );
    if ( mPlatformCL.empty() ) {
        throw Exception( "Не найдено ни одного устройства с поддержкой OpenCL." );
    }

    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM,
        ( cl_context_properties )( mPlatformCL.front() )(),
        0
    };
    mContextCL = cl::Context( CL_DEVICE_TYPE_CPU, properties );
    mDeviceCL = mContextCL.getInfo< CL_CONTEXT_DEVICES >();
    mQueueCL = cl::CommandQueue( mContextCL, mDeviceCL[ 0 ],  0,  &errorCL );

#ifdef _DEBUG
    std::cout << "\nВыбрано устройство OpenCL:\n";
    porte::printCLInfo( mPlatformCL.front()() );
#endif
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


    // собираем исходники ядер в один файл и компилируем
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # Последнее за "/" название является именем ядра.
        const std::string kernelKey = *itr;
        const std::string kernelName =
            itr->substr( itr->find_last_of( '/' ) + 1 );

        // Program Setup
        const std::string fileKernel = kernelKey + ".cl";
        const std::string pathAndName =
            L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/" + fileKernel;
#ifdef _DEBUG
        std::cout << "\"" << fileKernel << "\" ..";
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

        // @todo optimize Ядра можно загружать уже скомпилированные.
        const cl::Program::Sources  source( 1,  std::make_pair(
            kernelSourceCode.c_str(),  kernelSourceCode.length()
        ) );

        // компилируем каждое ядро в определённом контексте для устройства
#ifdef _DEBUG
        std::cout << "Опции OpenCL для ядра \"" << kernelKey << "\"" <<
            std::endl << commonOptions.str() <<
        std::endl;
#endif

        const cl::Program  program( mContextCL, source );
        try {
            program.build( mDeviceCL,  commonOptions.str().c_str() );

        } catch ( const cl::Error& ex ) {
            std::cerr << "(!) Error: " << ex.what() <<
                "(" << ex.err() << ")" <<
            std:: endl;
            if (ex.err() == CL_BUILD_PROGRAM_FAILURE) {
                std::cerr <<
                    program.getBuildInfo< CL_PROGRAM_BUILD_LOG >( mDeviceCL.front() ) <<
                std::endl;
            }
        }

        const cl::Kernel  kernel( program,  kernelName.c_str(),  &errorCL );
        mKernelCL[ kernelKey ] = kernel;

    } // for (auto itr

}




template< size_t GLOBAL_SIZE >
inline void EngineHybrid::enqueueEventKernelCL(
    const std::string&  key,
    cl::Event*          event
) {
    static const auto EMPTY_WAIT = vectorEventCL_t();
    enqueueEventKernelCL< GLOBAL_SIZE >(
        key,  EMPTY_WAIT,  event
    );
}




template< size_t GLOBAL_SIZE >
inline void EngineHybrid::enqueueEventKernelCL(
    const std::string&      key,
    const vectorEventCL_t&  waitEvents,
    cl::Event*              event
) {
    assert( (mKernelCL.find( key ) != mKernelCL.cend())
        && "Ядро OpenCL не найдено." );

    mQueueCL.enqueueNDRangeKernel(
        mKernelCL[ key ],
        cl::NullRange,
        cl::NDRange( GLOBAL_SIZE ),
        cl::NullRange,
        waitEvents.empty() ? nullptr : &waitEvents,
        event
    ); 
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
        << " -D UID_MODEL_LENGTH=" << pns::UID_MODEL_LENGTH

        // #! Если для вычислений не используется double, важно передавать
        //    вещественные значения как float. Иначе на драйвере OpenCL 1.2
        //    от Intel - ошибка компиляции.
        << std::scientific
        << " -D BIG_VALUE_BASE_0=" << pns::BIG_VALUE_BASE_0 << "f"
        << " -D BIG_VALUE_BASE_1=" << pns::BIG_VALUE_BASE_1 << "f"
        << " -D BIG_VALUE_BASE_2=" << pns::BIG_VALUE_BASE_2 << "f"
        << " -D BIG_VALUE_BASE_3=" << pns::BIG_VALUE_BASE_3 << "f"
        << " -D REAL_MAX=" << pns::REAL_MAX << "f"


        // точность сравнения значений с плав. точкой
        << " -D PRECISION=" << typelib::PRECISION << "f"

        // физические и геометрические константы
        << " -D PI=" << typelib::constant::pi << "f"
        << " -D SPEED_LIGHT=" << typelib::constant::physics::c << "f"
        << " -D CK_TEMPERATURE=" << typelib::constant::physics::CK << "f"
        << " -D G=" << typelib::constant::physics::G << "f"
        << " -D STEFAN_BOLTZMANN=" << typelib::constant::physics::stefanBoltzmann << "f"

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


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
