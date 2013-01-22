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

    // #! ���� ������ �������� �����������, �������� � ����������� ���������.
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
    // �������������� �������� � ������� ������ ��� ������ � OpenCL
    prepareCLContext();
    prepareCLCommandQueue();

    assert( gpuContextCL
        && "�������� OpenCL �� ���������������." );
    assert( commandQueueCL
        && "������� ������ OpenCL �� ����������������." );


    // �������������� ���� OpenCL (���� ������� ����������)
    prepare();
}




inline EngineHybrid::~EngineHybrid() {
    // ����������� ������, ���������� ��� ��������� ����
    if ( aboutStarSystemCL ) { clReleaseMemObject( aboutStarSystemCL ); }
    if ( asteroidCL ) { clReleaseMemObject( asteroidCL ); }
    if ( planetCL ) { clReleaseMemObject( planetCL ); }
    if ( starCL ) { clReleaseMemObject( starCL ); };

    // ������� ��������� ����
    for (auto itr = kernelCL.begin(); itr != kernelCL.end(); ++itr) {
        errorCL |= clReleaseKernel( itr->second );
    }
#ifdef _DEBUG
    oclCheckError( errorCL, CL_SUCCESS );
#endif

    // ��� ���������� ���� ������� ������ ���������, �����������
    free( devicesCL );

    // ����������� ������� ������ � ��������
    clReleaseCommandQueue( commandQueueCL );
    clReleaseContext( gpuContextCL );
}




void EngineHybrid::incarnate(
    std::shared_ptr< portulan_t >  p,
    real_t extentPortulan
) {
    EngineWithoutBooster::incarnate( p, extentPortulan );


    // �������������� ������ ��� ��������� ��� ���� OpenCL
    auto& topology = mPortulan.lock()->topology().topology();

    if ( aboutStarSystemCL ) { clReleaseMemObject( aboutStarSystemCL ); }
    aboutStarSystemCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeStarSystem,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        &topology.aboutStarSystem,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    if ( asteroidCL ) { clReleaseMemObject( asteroidCL ); }
    asteroidCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeAsteroid,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        topology.asteroid.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    if ( planetCL ) { clReleaseMemObject( planetCL ); }
    planetCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizePlanet,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        topology.planet.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    if ( starCL ) { clReleaseMemObject( starCL ); };
    starCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeStar,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        topology.star.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );    


    // �������������� ��������� ��� ����
#if 1
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/begin" ];

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
    }
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/star/begin" ];

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
    }
#endif


#if 1
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/direct" ];

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
    }
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/star/direct" ];

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
    }
#endif


#if 1
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/relative" ];

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
    }
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/star/relative" ];

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
    }
#endif


#if 1
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/fix" ];

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
    }
    {
        const cl_kernel kernel = kernelCL[ "set/emit-event/star/fix" ];

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
    }
#endif
}




inline void EngineHybrid::pulse( int n ) {

    assert( (n > 0)
        && "����� �������� ������ � ����������� ���-��� �������." );

    // ������������ ����������� �� ���� ������� �������
    // # ��� �������� ������� ������� �������� �� ������ �����.
    // # ������ ������� ������� ������� ���������� ���� ����� �������.
    // # ����������� ��������� ������� �������� � ������ ������� � ������
    //   ��������.
    // # ������� ����� �������� ����. ���������. �������, ����� "����������"
    //   ������� ������ ������� ������������� ��. �������� �� ������� �������,
    //   ������� ����� ������� ������� �� ��� �������.
    // # ����� �������� ������������ ������������ ������� (��������,
    //   ������������), ���������:
    //   �������. ����� ������� �������������� *������* ������ �� ��������
    //   ���������. ��������� ��������� ������������� ����� ������� � �������
    //   �� �� ������ ���������� (����) �������. �������� ������: ������
    //   �������� ������ ����� �������, ��������� ���������.

    // ��������� 'n' ������
    emitEvent( n );

    // ��� ���������� ������
    mLive.add( n,  mTimestep * n );

    // �������� ���������� ��� ��������� ���������
    statistics();
}




inline void EngineHybrid::emitEvent( int n ) {

    static const size_t grid = typelib::max(
        pns::ASTEROID_COUNT, pns::PLANET_COUNT, pns::STAR_COUNT
    );
    static const size_t GRID_WORK_DIM = 1;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid };


    /* @test
    const auto tss = sizeof( pns::aboutStarSystem_t );
    const auto ta = sizeof( pns::aboutAsteroid_t );
    const auto tp = sizeof( pns::aboutPlanet_t );
    const auto ts = sizeof( pns::aboutStar_t );

    const auto tca = sizeof( pns::characteristicAsteroid_t );
    const auto tc = sizeof( pns::coord_t );
    const auto tr1 = sizeof( pns::real_t );
    const auto tr2 = sizeof( pns::real2_t );
    const auto tr4 = sizeof( pns::real4_t );
    const auto tb = sizeof( bool );

    const auto te2 = sizeof( pns::eventTwo_t );
    const auto tpe = sizeof( pns::pointerElement_t );
    const auto te = sizeof( enum pns::EVENT );
    */


    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;


    // �������� 'n' �������
    // # ��������� ��� ���� ��� ������������ � incarnate().
    for (int p = 0; p < n; ++p) {

    // # ����� ������� ������� ������ � � ����� ���� �������� -
    //   ����������� (��������������). ��. �������� ����.

    // �������������� �������� � �������� �������
#if 1
    {
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/begin" ];
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
        }
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/star/begin" ];
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
        }

        // �������������
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // �������� ���� �������
#if 1
    {
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/direct" ];
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
        }
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/star/direct" ];
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
        }

        // �������������
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

#if 0
        // @test ������������� ���������
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
#endif


    // �������� ��������� �������
#if 1
    {
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/relative" ];
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
        }
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/star/relative" ];
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
        }

        // �������������
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

#if 0
        // @test ������������� ���������
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
#endif


    // ���������� ���������� � �������� � ������� �������
    // ��� �� �������� ���������� � ������������� ����������� �������
#if 1
    // ������� ����������
#if 1
    bool needOptimizeAsteroid = false;
    // ��������� ������ �� �����
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

    // ������������� �������
    for (size_t i = 0;
        // # ����� ����� ��������������� ��������� "������ ������� -
        //   ������ ��� �������", �.�. �������� � ������ �������
        //   ������ - �����������. ��. ���������� � ������ ������.
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

            // ���������� �����������
            for (auto etr = StoreListenerAsteroid::begin();
                etr;  etr = StoreListenerAsteroid::next()
            ) { if ( etr ) {
                const auto& l = etr->listener.lock();
                l->beforeEvent( piA, e );
                // @todo ��������� ������� ����� 'switch', ��� � ����� OpenCL.
            } }

            // �������, ����������� �� ����� ����� ������� �����������
            if (e.uid == pns::E_DESTROY) {
                needOptimizeAsteroid = true;
            }

        } // for (int w = ...

    } // for (size_t i = ...
#endif

    // ������� ������
#if 1
    bool needOptimizePlanet = false;
    // @todo ...
#endif

    // ������� ����
#if 1
    bool needOptimizeStar = false;
    // ��������� ������ �� �����
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

    // ������������� �������
    for (size_t i = 0;
        // # ����� ����� ��������������� ��������� "������ ������� -
        //   ������ ��� �������", �.�. �������� � ������ �������
        //   ������ - �����������. ��. ���������� � ������ ������.
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

            // ���������� �����������
            for (auto etr = StoreListenerStar::begin();
                etr;  etr = StoreListenerStar::next()
            ) { if ( etr ) {
                const auto& l = etr->listener.lock();
                l->beforeEvent( piA, e );
                // @todo ��������� ������� ����� 'switch', ��� � ����� OpenCL.
            } }

            // �������, ����������� �� ����� ����� ������� �����������
            if (e.uid == pns::E_DESTROY) {
                needOptimizeStar = true;
            }

        } // for (int w = ...

    } // for (size_t i = ...
#endif

#endif


    // ��������� �������������� �������� ����� ��������
#if 1
    {
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/asteroid/fix" ];
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
        }
        {
            const cl_kernel kernel = kernelCL[ "set/emit-event/star/fix" ];
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
        }

        // �������������
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    }
#endif


    // # ���� ���������� ������� "���������� ������ ������� - ������
    //   ����� �� �������������". ������� ��-�� �������, �������� �������
    //   "������� �����������" - ��. pns::absent*() - ��������� ��������������
    //   ������ ��������� ��� ����. ������.
    // # ����������� ������ ���� ����� ��������� �������, �.�. ���� �����
    //   ������������ i-������� ��� ������� � �������������� ���������� ��
    //   ��������. @todo ? ����� �� ����� ���������� - ����������� ������ ���
    //   �� ��������� ���������� �� �����.
    // ����������� - ����������� ��������, ��������� �� ������������
#if 1

    // ����������� ������ ����������
#if 1
    // @todo optimize fine �������������� � ���� OpenCL.
    if ( needOptimizeAsteroid ) {
        // ������� ������ �� �����
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

        // ������������
        pns::optimizeCountAsteroid( asteroid );

        // ������ ������ �� ����������
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

    // ����������� ������ ������
#if 1
    // @todo optimize fine �������������� � ���� OpenCL.
    if ( needOptimizePlanet ) {
        // ������� ������ �� �����
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

        // ������������
        pns::optimizeCountPlanet( planet );

        // ������ ������ �� ����������
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

    // ����������� ������ ����
#if 1
    // @todo optimize fine �������������� � ���� OpenCL.
    if ( needOptimizeStar ) {
        // ������� ������ �� �����
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

        // ������������
        pns::optimizeCountStar( star );

        // ������ ������ �� ����������
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

    } // for (int p = 0; p < n; ++p)


    // ���������
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
    // (!) ���� ���������� ������� ������� ������ �����, OpenCL �� ����� ���������������.
    //     ��������� LARGEADDRESSAWARE �� ������ ��������.
    gpuContextCL = clCreateContext(
        props, 1, &devicesCL[ deviceUsedCL ],
        &pfn_notify_cl, nullptr, &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "������� ���������� (OpenCL):" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif

}





inline void EngineHybrid::prepareCLCommandQueue() {
    assert( gpuContextCL
        && "�������� OpenCL ��������� ���������������� �� ���������� ����� ������." );

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
    // # �������� � ������� ������ ���������������� � ������������.

#ifdef _DEBUG
        std::cout << std::endl << std::endl;
#endif

    std::ostringstream commonOptions;
    commonOptions
        // ����� �������� ��� float
        << std::fixed
        // #! ������� ����� ������������� ��������: ���� ������
        //   � helper.hcl, ������� ���������� ��.
        << " " << options
        << commonConstantCLKernel()
        << commonOptionCLKernel()
        << "";

    // ����� ����� ��� ����������� ���� ���������
    std::string kernelLibraryCode = "";

    // ���������� ����� ���������� � ���������
    // (������������ ����� ������ ������)
    // #! ����� ������� �����������.
    std::vector< std::string > hcl = boost::assign::list_of
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/set/pragma.hcl" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/set/restruct.hcl" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/set/helper.hcl" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/structure.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/asteroid.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/planet.h" )
        ( L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/star.h" )
        // ��� h-����� ���� ��������, �.�. ��� �������� �
        // star-system.h (������������ �� ���� �����)
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
        std::cout << "�������� \"" << shortName << "\" ..";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        assert( file.is_open()
            && "���� �� ������." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        kernelLibraryCode +=
            (std::string)"\n\n\n\n\n" +
            "// @include " + pathAndName + "\n" +
            buffer.str();
#ifdef _DEBUG
    std::cout << " ��" << std::endl;
#endif
    } // for (auto itr = hcl.cbegin(); ...


    // ����������� ����
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # ��������� �� "/" �������� �������� ������ ����.
        const std::string kernelKey = *itr;
        const std::string kernelName = itr->substr( itr->find_last_of( '/' ) + 1 );

        // Program Setup
        const std::string fileKernel = kernelKey + ".cl";
        const std::string pathAndName =
            L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/" + fileKernel;
#ifdef _DEBUG
        std::cout << "�������� \"" << fileKernel << "\" ..";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        assert( file.is_open()
            && "���� ���� �� ������." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        const std::string kernelSC = buffer.str();
#ifdef _DEBUG
    std::cout << " ��" << std::endl;
#endif

        // create the program
        const std::string kernelSourceCode =
            kernelLibraryCode + "\n\n\n\n\n" +
            "// @include kernel " + pathAndName + "\n" +
            kernelSC;

#ifdef _DEBUG
        // ��������� ������ ��� ���� � ����
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
        std::cout << "����� OpenCL ��� ���� \"" << kernelKey << "\"" << std::endl << commonOptions.str() << std::endl;
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

        // ��������� ������� ���� ���������� ����� ������
        clReleaseProgram( programCL );

    } // for (auto itr

}





inline std::string EngineHybrid::commonConstantCLKernel() {

    std::ostringstream options;
    options
        // ����� �������� ��� float
        << std::fixed

        << " -D __DEBUG"
        //<< " -D GPU_OPENCL"

        << " -D ASTEROID_COUNT=" << pns::ASTEROID_COUNT
        << " -D PLANET_COUNT=" << pns::PLANET_COUNT
        << " -D STAR_COUNT=" << pns::STAR_COUNT
        << " -D EMITTER_EVENT_COUNT=" << pns::EMITTER_EVENT_COUNT
        << " -D MAX_FEATURE_EVENT=" << pns::MAX_FEATURE_EVENT

        << std::scientific
        << " -D BIG_VALUE_BASE_0=" << pns::BIG_VALUE_BASE_0
        << " -D BIG_VALUE_BASE_1=" << pns::BIG_VALUE_BASE_1
        << " -D BIG_VALUE_BASE_2=" << pns::BIG_VALUE_BASE_2
        << " -D BIG_VALUE_BASE_3=" << pns::BIG_VALUE_BASE_3

        // �������� ��������� �������� � ����. ������
        << " -D PRECISION=" << typelib::PRECISION

        // ���������� � �������������� ���������
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
        // ����� �������� ��� float
        //<< std::fixed

        // �������������� � ����� ������� ��������
        << " -Werror"
#if 0
        // ��������� �����������
        // (i) ~10% ������� ������ � ���������� ����� ����.
        // @todo optimize ������ ����������� OpenCL.
        //       http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
        << " -cl-fast-relaxed-math"
        << " -cl-mad-enable"
#endif
#if 0
        // ������������ �������
        // (i) �������� ������ ��� ������������ ����: ����� ���
        //     20-������� ����������.
        << " -cl-opt-disable"
#endif
        << "";

    return options.str();
}





inline void EngineHybrid::fnErrorCL( int exitCode ) {
    std::cerr << "��� ������ OpenCL: " << exitCode << std::endl;

    // @todo fine ����������� ����������.
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
