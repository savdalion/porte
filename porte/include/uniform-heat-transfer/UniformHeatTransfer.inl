namespace porte {


template< size_t SX, size_t SY, size_t SZ >
inline UniformHeatTransfer< SX, SY, SZ >::UniformHeatTransfer(
    portulan_t* p
) :
    Engine( p ),
    gpuContextCL( nullptr ),
    commandQueueCL( nullptr ),
    boosterCL( nullptr ),
    workBoosterCL( nullptr )
{
    // �������������� ���� OpenCL (���� ������� ����������)
    prepare();
}





template< size_t SX, size_t SY, size_t SZ >
inline UniformHeatTransfer< SX, SY, SZ >::~UniformHeatTransfer() {

    /* - ���: ����� ��� OpenCL ������ �� ��������� ����� ���������...
    // ����������� ��������� ��� ������ ������� � OpenCL
    clReleaseMemObject( boosterCL );
    */
    // ...�� ��������� ��������� - ����������
    clReleaseMemObject( workBoosterCL );

    // ������� ��������� ����
    cl_int errorCL = CL_SUCCESS;
    for (auto itr = kernelCL.begin(); itr != kernelCL.end(); ++itr) {
        errorCL |= clReleaseKernel( itr->second );
    }
    //errorCL |= clReleaseProgram( programCL );
#ifdef _DEBUG
    oclCheckError( errorCL, CL_SUCCESS );
#endif

    // ����������� ������� ������ � ��������
    clReleaseCommandQueue( commandQueueCL );
    clReleaseContext( gpuContextCL );
}







template< size_t SX, size_t SY, size_t SZ >
inline void UniformHeatTransfer< SX, SY, SZ >::pulse( int n ) {
    // (!) ����� ��� ������ ���� ���������������� � ������-����������.
    // (!) ��������� ��� �������� OpenCL ������ ���� ������������ � prepare().

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
        // @todo optimize ��������� ����� ����� �������� ���� ��� ����� �����.
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

}








#if 0
// - ���������� ����� oclUtils. ��. ����.

// @source ������ "v3d".
template< size_t SX, size_t SY, size_t SZ >
inline void UniformHeatTransfer< SX, SY, SZ >::prepare() {

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







template< size_t SX, size_t SY, size_t SZ >
inline void UniformHeatTransfer< SX, SY, SZ >::prepare() {

    // �������������� ���� OpenCL
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
    // (!) ���� ���������� ������� ������� ������ �����, OpenCL �� ����� ���������������.
    //     ��������� LARGEADDRESSAWARE �� ������ ��������.
    gpuContextCL = clCreateContext( props, 1, &devicesCL[ deviceUsedCL ], nullptr, nullptr, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "������� ���������� (OpenCL):" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif


    // create a command-queue
    commandQueueCL = clCreateCommandQueue( gpuContextCL, devicesCL[ deviceUsedCL ], 0, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // ����������� �������������� ����

    const std::vector< std::string > kernelNames = boost::assign::list_of
        ( "calcUniformHeatTransfer" )
        ( "fixResult" )
    ;
    const std::string searchPath =
        PATH_CL_PORTE + "/uniform-heat-transfer/set/calc";
    for (auto itr = std::begin( kernelNames ); itr != std::end( kernelNames ); ++itr) {
        const std::string kernelName = *itr;
        // Program Setup
        size_t programLength;
        const std::string fileKernel = kernelName + ".cl";
        /* - �����. ��. ����.
        char* pathAndNameCL = shrFindFilePath( fileKernel.c_str(),  searchPath.c_str() );
        oclCheckErrorEX( (pathAndNameCL != nullptr), true, &fnErrorCL );
        */
        const std::string pathAndName = searchPath + "/" + fileKernel;
        const char* pureSourceCode = oclLoadProgSource( pathAndName.c_str(), "", &programLength );
        oclCheckErrorEX( (pureSourceCode != nullptr), true, &fnErrorCL );

        // ��������� � ����� ���������� �����, ����� OpenCL �� �����
        // ����� ����������� ����� ��������� �� ���� ����������: �����
        // ������� �������� ������������� ������, ����� ��������� ��
        // *���������� ������* ����������� "����� ���"
#if 0
        // - �����. ��. ����.
        const std::string sourceCode = static_cast< std::string >( pureSourceCode ) +
            "\n/*" + boost::lexical_cast< std::string >( boost::posix_time::second_clock::local_time() ) + "*/";
#endif
        //const std::string randstamp = boost::lexical_cast< std::string >( std::rand() % 100000 );
        const std::string randstamp = boost::lexical_cast< std::string >( static_cast< unsigned int >( time( nullptr ) ) );
        const std::string sourceCode = static_cast< std::string >( pureSourceCode ) + "\n/*" + randstamp + "*/";

        const char* sourceCL = sourceCode.c_str();

        // create the program
        cl_program programCL = clCreateProgramWithSource(
            gpuContextCL,  1,  (const char**)&sourceCL,  &programLength, &errorCL
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    

        // �������������� ��������� ��� ����
        typedef portulan::Portulan< SX, SY, SZ >::topology_t::numberLayer_t  nl_t;
        std::ostringstream options;
        options
            // ����� �������� ��� float
            << std::fixed

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
            << " -D PRECISION=" << typelib::PRECISION

            /* - @todo ���������� ���������
            // ������� �� �������� � �������
            << " -D KELVIN_CELSIUS_TEMPERATURE=-273.15f"
            // ������� ����������, �� / (���� * K)
            // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
            << " -D R_GAS=8.31441f"
            */

            // ���� � ���������� ������ (*.hcl)
            << " -I " << searchPath
            // �������������: �������������� � ����� ������� ��������
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

            // ��������� � ���������� (� 2012 �. ������� OpenCL �� NVIDIA
            // �������� ���������� ���� ��� ����� ������������) ����������
            // �����, ����� OpenCL �� ����� ����� ����������� ����� ���������
            // �� ���� ����������: ����� ������� �������� ������������� ������,
            // ����� ��������� �� *���������� ������* ����������� "����� ���"
            << " -D BUILD_RANDSTAMP=" << randstamp

            << "";
    
#ifdef _DEBUG
        // @test
        std::cout << "����� OpenCL ��� ���� \"" << kernelName << "\":" << std::endl << options.str() << std::endl;
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

        // ��������� ������� ���� ���������� ����� ������
        clReleaseProgram( programCL );

    } // for (auto itr



    // �������������� ��������� ��� ������ � OpenCL

#ifdef _DEBUG
    // @test
    const size_t sb = sizeof( mPortulanBooster.booster() );
#endif

    boosterCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        sizeof( mPortulanBooster.booster() ),
        &mPortulanBooster.booster(),
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    workBoosterCL = clCreateBuffer(
        gpuContextCL,
        // �������� ������ ��� ��������� ���������
        // (������������ ������ OpenCL)
        CL_MEM_READ_WRITE,
        sizeof( mPortulanBooster.booster() ),
        nullptr,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}






template< size_t SX, size_t SY, size_t SZ >
inline void UniformHeatTransfer< SX, SY, SZ >::fnErrorCL( int exitCode ) {
    std::cerr << "OpenCL �� ���������������. ��� ������: " << exitCode << std::endl;

    // @todo fine ����������� ����������.
    std::cin.ignore();
    std::exit( exitCode );
}



} // porte
