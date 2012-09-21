namespace porte {


inline DungeonCrawl::DungeonCrawl(
    portulan_t* p
) :
    EngineWithoutBooster( p ),

    aboutPlanetCL( nullptr ),
    memsizeAboutPlanet( sizeof( portulan::planet::set::dungeoncrawl::aboutPlanet_t ) ),

    componentCL( nullptr ),
    workComponentCL( nullptr ),
    // #! ���� ������ �������� �����������, �������� � ����������� ���������.
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

    // �������������� �������� � ������� ������ ��� ������ � OpenCL
    prepareCLContext();
    prepareCLCommandQueue();

    assert( gpuContextCL
        && "�������� OpenCL �� ���������������." );
    assert( commandQueueCL
        && "������� ������ OpenCL �� ����������������." );


    // �������������� ����. ��������� ��� ���� OpenCL

    // aboutPlanet
    aboutPlanetCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeAboutPlanet,
        &mPortulan->topology().topology().aboutPlanet,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // component
    componentCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeComponent,
        // #! ���� ������ �������� �����������, ���������� � ����������.
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
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
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


    // �������������� ���� OpenCL (���� ������� ����������)
    prepareCLKernel();
}





inline DungeonCrawl::~DungeonCrawl() {

    /* - ���: ����� ��� OpenCL ������ �� ��������� ����� ���������...
    // ����������� ��������� ��� ������ ������� � OpenCL
    clReleaseMemObject( aboutPlanetCL );
    ...
    */
    // ...�� ��������� ��������� - ����������
    clReleaseMemObject( workComponentCL );
    clReleaseMemObject( workLivingCL );
    clReleaseMemObject( workTemperatureCL );

    // ������� ��������� ����
    for (auto itr = kernelCL.begin(); itr != kernelCL.end(); ++itr) {
        errorCL |= clReleaseKernel( itr->second );
    }
#ifdef _DEBUG
    oclCheckError( errorCL, CL_SUCCESS );
#endif

    // ����������� ������� ������ � ��������
    clReleaseCommandQueue( commandQueueCL );
    clReleaseContext( gpuContextCL );
}




inline void DungeonCrawl::init() {
    initComponent();
    initLiving();
    initTemperature();
}




inline void DungeonCrawl::initComponent() {
    // #! ��������� ��� �������� OpenCL ������ ���� ������������ � prepareComponentCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pc = portulan::planet::set::dungeoncrawl::component;

    static const size_t grid = pd::COMPONENT_GRID;

    // ����������� �����
    static const size_t GRID_WORK_DIM = 3;
    // ���������� Work Item
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };
    // ������ Work Item
    /* - ����� OpenCL ������� ������ ������ ���.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( ... );
    */
    // ����� � ������� ������ = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    // �������������� �����
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    // #! ��������� ��� �������� OpenCL ������ ���� ������������ � prepareTemperatureCLKernel().

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pt = portulan::planet::set::dungeoncrawl::temperature;

    static const size_t grid = pd::TEMPERATURE_GRID;

    // ����������� �����
    static const size_t GRID_WORK_DIM = 3;
    // ���������� Work Item
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };
    // ������ Work Item
    /* - ����� OpenCL ������� ������ ������ ���.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( ... );
    */
    // ����� � ������� ������ = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    // �������������� �����
    const cl_kernel kernel = kernelCL[ "scale/temperature/top/init" ];


    /* @test
    pd::test_t test;
    const size_t memsizeTest = sizeof( pd::test_t );
    cl_mem testCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    // (!) ����� ��� ������ ���� ���������������� � ������-����������.
    // (!) ��������� ��� �������� OpenCL ������ ���� ������������ � prepareCLKernel().

    cl_int errorCL = CL_SUCCESS;

    // ����������� �����
    const size_t GRID_WORK_DIM = 3;
    // ���������� Work Item
    const size_t GRID_GLOBAL_WORK_SIZE[] = { SX, SY, SZ };
    // ������ Work Item
    /* - ����� OpenCL ������� ������ ������ ���.
    const cl::NDRange GRID_LOCAL_WORK_COUNT( 3, 3, 3 );
    */
    // ����� � ������� ������ = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT

#ifdef _DEBUG
    // @test
    const size_t sizeT = sizeof( cl_mem );
    const size_t sizeW = sizeof( cl_mem );
#endif

    // ��������������...

#ifdef _DEBUG
    /* @test
    const size_t nb = std::end( mPortulanBooster.booster().temperature ) -
                      std::begin( mPortulanBooster.booster().temperature );
    const size_t sbe = sizeof( mPortulanBooster.booster() );
    std::cout << "����� ����������� \"��\": "
        << std::accumulate(
            std::begin( mPortulanBooster.booster().temperature ),
            std::end( mPortulanBooster.booster().temperature ),
            0.0f
        ) << std::endl;
    */
#endif


    // ��������� ��������� ���-�� �������
    for (int q = 0; q < n; ++q) {

        // ����������...

        // 1. ��������� ������� �����������, ������� �� ��������� �����.
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

        // �������������
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


        // 2. �������� ��������� �� ��������� ������� � ������� ����������.
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

        // �������������
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    
    } // for (int q = 0; q < n; ++q)


    // �������� ��������� ����� "n" �������...
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
    std::cout << "����� ����������� \"�����\": "
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
// - ���������� ����� oclUtils. ��. ����.

// @source ������ "v3d".
template< size_t SX, size_t SY, size_t SZ >
inline void UniformHeatTransfer< SX, SY, SZ >::prepareCLKernel() {

    // @source http://www10.informatik.uni-erlangen.de/Teaching/Courses/SS2010/SiWiR2/teaching/siwir2-lecture07-4on1.pdf
    // @source http://developer.amd.com/gpu_assets/OpenCL_Parallel_Computing_for_CPUs_and_GPUs_201003.pdf

    namespace fs = boost::filesystem;

    typedef portulan::Portulan< SX, SY, SZ >::numberLayer_t  nl_t;

    // ����������� �����
    const size_t GRID_WORK_DIM = 3;
    // ���������� Work Item
    const size_t GRID_GLOBAL_WORK_SIZE[] = { SX, SY, SZ };
    // ������ Work Item
    /* - ����� OpenCL ������� ������ ���
    const cl::NDRange GRID_LOCAL_WORK_COUNT(
        mSizeWorld.get<0>() / mSizeArea.get<0>(),  // = 3
        mSizeWorld.get<1>() / mSizeArea.get<1>(),  // = 3
        mSizeWorld.get<2>() / mSizeArea.get<2>()   // = 3
    );
    */
    // ����� � ������� ������ = GRID_GLOBAL_WORK_SIZE / GRID_LOCAL_WORK_COUNT


    // �������������� ���� OpenCL
    /* - �� ���������� assert-��������: ������ �� �������� ����� ����������.
         �������: ���� ���.
    cl_int error = CL_SUCCESS;
    */
    cl::Program program;

    // �������������� ��������� ��� ����
    std::ostringstream options;
    options
        // ������ ����
        << " -D N=" << SX
        << " -D M=" << SY
        << " -D L=" << SZ
        // ����������� � ������������ ����������
        << " -D MINX=" << nl_t::minCoord().x
        << " -D MINY=" << nl_t::minCoord().y
        << " -D MINZ=" << nl_t::minCoord().z
        << " -D MAXX=" << nl_t::maxCoord().x
        << " -D MAXY=" << nl_t::maxCoord().y
        << " -D MAXZ=" << nl_t::maxCoord().z
        // ��������� ��� ��������� ������ ����
        << " -D NM="  << ( SX * SY )
        << " -D NML=" << ( SX * SY * SZ )

        // �������� ��������� �������� � ����. ������
        //<< " -D PRECISION=" << typelib::PRECISION

        /* - @todo ���������� ���������
        // ������� �� �������� � �������
        << " -D KELVIN_CELSIUS_TEMPERATURE=-273.15f"
        // ������� ����������, �� / (���� * K)
        // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
        << " -D R_GAS=8.31441f"
        */

        // ���� � ���������� ������ (*.hcl)
        << " -I " << PATH_OPENCL
        // �������������: �������������� � ����� ������� ��������
        << " -Werror"

        // @todo optimize �������� ����������� OpenCL.
        //       "-cl-fast-relaxed-math", "-cl-mad-enable"
        //       � ��.. - ��. http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html
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
            std::cerr << std::endl << "��������� �� �������." << std::endl;
            throw "Platform is not found.";
        }
 
        cl_context_properties properties[] =
           { CL_CONTEXT_PLATFORM, (cl_context_properties)( platforms[0] )(), 0 };
        contextCL = cl::Context( CL_DEVICE_TYPE_GPU, properties, nullptr, nullptr );
  
        deviceCL = contextCL.getInfo< CL_CONTEXT_DEVICES >();
        assert( !deviceCL.empty() && "�� ������� �� ������ ���������� � ���������� OpenCL." );
  

        // �������� �������� ��� ���������
        assert( fs::exists( PATH_OPENCL ) && "����� � �������� ����� OpenCL �� ����������." );
        assert( fs::is_directory( PATH_OPENCL ) && "���� � ������ OpenCL �� �������� �������� �����������." );

        cl::Program::Sources sources;

        // �������� � ����� ����������, � ������� �������� ���� ������
        const std::string f = PATH_OPENCL + "kernel.cl";
        assert( fs::exists( f ) && "���� 'kernel.cl' �� ������." );
        std::ifstream fstr( f );
        const std::string str = std::string(
            std::istreambuf_iterator< char >( fstr ),
            std::istreambuf_iterator< char >()
            // ��������� � ����� ���������� �����, ����� OpenCL �� �����
            // ����� ����������� ����� ��������� �� ���� ����������: �����
            // ������� �������� ������������� ������, ����� ��������� ��
            // *���������� ������* ����������� "����� ���"
        ) + "\n/*" + boost::lexical_cast< std::string >( boost::posix_time::second_clock::local_time() ) + "*/";
        sources.push_back( std::make_pair( str.c_str(), str.length() + 1 ) );


        // �������� ���������
        program = cl::Program( contextCL, sources );
        program.build( deviceCL, options.str().c_str() );
  
        // ����������� ����
        VECTOR_CLASS< cl::Kernel >  kernels;
        program.createKernels( &kernels );

        // ���������������� ���� �������� � ����� ��� ����������� �������������
        for (auto itr = kernels.cbegin(); itr != kernels.cend(); ++itr) {
            const std::string name =
                (std::string)itr->getInfo< CL_KERNEL_FUNCTION_NAME >();
            kernelCL[ name ] = *itr;
        }

        // �������������� ������� ��� ������
        queueCL = cl::CommandQueue( contextCL, deviceCL[0] );
  

        /* - @example ���������� ���� � ��������� ����������.
        cl::Event event;
        //const cl::Kernel& kernel = kernelCL.cbegin()->second;
        const cl::Kernel& kernel = kernelCL[ "calcVisualHideMap" ];
        queueCL.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            GRID_GLOBAL_WORK_SIZE,
            // @todo optimize ���������� cl::NullRange - OpenCL ��� ���������
            //       ��������� ����������� ���-�� ������� �����.
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
          && "����������� ���������� ������ ������ ���� ��������� �������������� ���� OpenCL." );
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
    // (!) ���� ���������� ������� ������� ������ �����, OpenCL �� ����� ���������������.
    //     ��������� LARGEADDRESSAWARE �� ������ ��������.
    gpuContextCL = clCreateContext( props, 1, &devicesCL[ deviceUsedCL ], nullptr, nullptr, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "������� ���������� (OpenCL):" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif

}





inline void DungeonCrawl::prepareCLCommandQueue() {
    assert( gpuContextCL
        && "�������� OpenCL ��������� ���������������� �� ���������� ����� ������." );

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

    // # �������� � ������� ������ ���������������� � ������������.

    // ����������� ���� OpenCL

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;
    // @todo ������ �� ������ � "scale". ������ - ������������� ����, �����.
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # ��������� �� "/" �������� �������� ������ ����.
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
    

        // �������������� ��������� ��� ����
        static const size_t grid = pd::COMPONENT_GRID;
        typedef typelib::StaticMapContent3D< grid, grid, grid >  smc_t;

        // ������� ��� ������� ����� ������� �����, ����� �� ��������� ����
        // # ������� - ������� ������ �������� 1 ������ ������� �����.
        const float scale =
            mPortulan->topology().topology().aboutPlanet.size /
            static_cast< float >( grid );

        std::ostringstream options;
        options
            // ����� �������� ��� float
            << std::fixed
            // #! ������� ����� ������������� ��������: ���� ������
            //   � helper.hcl, ������� ���������� ��.
            << " -D GRID=" << grid
            << " -D MIN_COORD_GRID=" << smc_t::minCoord().x
            << " -D MAX_COORD_GRID=" << smc_t::maxCoord().x
            << " -D SCALE=" << scale
            << commonConstantCLKernel()
            << commonOptionCLKernel()
            << "";
    
#ifdef _DEBUG
        // @test
        std::cout << "����� OpenCL ��� ����� ���� \"" << kernelKey << "\":" << std::endl << options.str() << std::endl;
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

        // ��������� ������� ���� ���������� ����� ������
        clReleaseProgram( programCL );

    } // for (auto itr

}





inline void DungeonCrawl::prepareLivingCLKernel() {
    // @todo ...
}






inline void DungeonCrawl::prepareTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace pt = portulan::planet::set::dungeoncrawl::temperature;

    // # �������� � ������� ������ ���������������� � ������������.

    // ����������� ���� OpenCL

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/temperature/top/init" )
    ;
    // @todo ������ �� ������ � "scale". ������ - ������������� ����, �����.
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # ��������� �� "/" �������� �������� ������ ����.
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
    

        // �������������� ��������� ��� ����
        static const size_t grid = pd::TEMPERATURE_GRID;
        typedef typelib::StaticMapContent3D< grid, grid, grid >  smc_t;

        // ������� ��� ������� ����� ������� �����, ����� �� ��������� ����
        // # ������� - ������� ������ �������� 1 ������ ������� �����.
        const float scale =
            mPortulan->topology().topology().aboutPlanet.size /
            static_cast< float >( grid );

        std::ostringstream options;
        options
            // ����� �������� ��� float
            << std::fixed
            // #! ������� ����� ������������� ��������: ���� ������
            //   � helper.hcl, ������� ���������� ��.
            << " -D GRID=" << grid
            << " -D MIN_COORD_GRID=" << smc_t::minCoord().x
            << " -D MAX_COORD_GRID=" << smc_t::maxCoord().x
            << " -D SCALE=" << scale
            << commonConstantCLKernel()
            << commonOptionCLKernel()
            << "";
    
#ifdef _DEBUG
        // @test
        std::cout << "����� OpenCL ��� ����� ���� \"" << kernelKey << "\":" << std::endl << options.str() << std::endl;
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

        // ��������� ������� ���� ���������� ����� ������
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

    // ��������� ��� ���������� ���������� ��������� ��� �����
    typedef typelib::StaticMapContent3D< pd::COMPONENT_GRID, pd::COMPONENT_GRID, pd::COMPONENT_GRID >        componentSMC_t;
    typedef typelib::StaticMapContent3D< pd::LIVING_GRID, pd::LIVING_GRID, pd::LIVING_GRID >                 livingSMC_t;
    typedef typelib::StaticMapContent3D< pd::TEMPERATURE_GRID, pd::TEMPERATURE_GRID, pd::TEMPERATURE_GRID >  temperatureSMC_t;

    static std::ostringstream options;
    options
        // ����� �������� ��� float
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

        // �������� ��������� �������� � ����. ������
        << " -D PRECISION=" << typelib::PRECISION

        // ���������� ���������
        // ������� �� �������� � �������
        << " -D CK_TEMPERATURE=" << typelib::constant::physics::CK
        /* - @todo ...
        // ������� ����������, �� / (���� * K)
        // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
        << " -D R_GAS=8.31441f"
        */

        // ��������� � ���������� (� 2012 �. ������� OpenCL �� NVIDIA
        // �������� ���������� ���� ��� ����� ������������) ����������
        // �����, ����� OpenCL �� ����� ����� ����������� ����� ���������
        // �� ���� ����������: ����� ������� �������� ������������� ������,
        // ����� ��������� �� *���������� ������* ����������� "����� ���"
        << " -D BUILD_RANDSTAMP=" << randstamp

        << "";

    return options.str();
}





inline std::string DungeonCrawl::commonOptionCLKernel() {
    static std::ostringstream options;
    options
        // ����� �������� ��� float
        //<< std::fixed

        // ���� � ���������� ������ (*.hcl)
        << " -I " << PATH_CL_DUNGEONCRAWL
        // �������������� � ����� ������� ��������
        << " -Werror"
#if 0
        // ��������� �����������
        // (i) ~10% ������� ������ � ���������� ����� ����.
        // @todo optimize ������ ����������� OpenCL.
        //       http://khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html
        << " -cl-fast-relaxed-math"
        << " -cl-mad-enable"
#endif
#if 0
        // ������������ �������
        // (i) �������� ������� ������ ��� ������������ ����: ����� ���
        //     20-������� ���������� 
        << " -cl-opt-disable"
#endif
        << "";

    return options.str();
}





inline void DungeonCrawl::fnErrorCL( int exitCode ) {
    std::cerr << "��� ������ OpenCL: " << exitCode << std::endl;

    // @todo fine ����������� ����������.
    std::cin.ignore();
    std::exit( exitCode );
}



} // porte
