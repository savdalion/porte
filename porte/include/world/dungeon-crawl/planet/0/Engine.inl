namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;


inline Engine::Engine(
    real_t timestep
) :
    EngineWithoutBooster( timestep ),

    aboutPlanetCL( nullptr ),
    memsizeAboutPlanet( sizeof( pnp::aboutPlanet_t ) ),

    //componentCL( nullptr ),
    // #! ���� ������ �������� �����������, �������� � ����������� ���������.
    memsizeComponent( sizeof( pnp::componentCell_t ) *
        pnp::COMPONENT_GRID *
        pnp::COMPONENT_GRID *
        pnp::COMPONENT_GRID
    ),

    memsizeTemperature( sizeof( pnp::temperatureCell_t ) *
        pnp::TEMPERATURE_GRID *
        pnp::TEMPERATURE_GRID *
        pnp::TEMPERATURE_GRID
    ),

    memsizeSurfaceTemperature( sizeof( pnp::surfaceTemperatureCell_t ) *
        pnp::SURFACE_TEMPERATURE_GRID *
        pnp::SURFACE_TEMPERATURE_GRID *
        pnp::SURFACE_TEMPERATURE_GRID
    ),

    memsizeRainfall( sizeof( pnp::rainfallCell_t ) *
        pnp::RAINFALL_GRID *
        pnp::RAINFALL_GRID *
        pnp::RAINFALL_GRID
    ),

    memsizeDrainage( sizeof( pnp::drainageCell_t ) *
        pnp::DRAINAGE_GRID *
        pnp::DRAINAGE_GRID *
        pnp::DRAINAGE_GRID
    ),

    memsizeLandscape( sizeof( pnp::landscapeCell_t ) *
        pnp::LANDSCAPE_GRID *
        pnp::LANDSCAPE_GRID *
        pnp::LANDSCAPE_GRID
    ),

    memsizeIlluminance( sizeof( pnp::illuminanceCell_t ) *
        pnp::ILLUMINANCE_GRID *
        pnp::ILLUMINANCE_GRID *
        pnp::ILLUMINANCE_GRID
    ),

    memsizeBiome( sizeof( pnp::biomeCell_t ) *
        pnp::BIOME_GRID *
        pnp::BIOME_GRID *
        pnp::BIOME_GRID
    ),

    memsizeLiving( sizeof( pnp::livingCell_t ) *
        pnp::LIVING_GRID *
        pnp::LIVING_GRID *
        pnp::LIVING_GRID
    ),

    // ����������������� ��������� ����. �����
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
    // �������������� �������� � ������� ������ ��� ������ � OpenCL
    prepareCLContext();
    prepareCLCommandQueue();

    assert( gpuContextCL
        && "�������� OpenCL �� ���������������." );
    assert( commandQueueCL
        && "������� ������ OpenCL �� ����������������." );

    // ��������� ����� ��� ���������� ����. �����
    randomCore.seed( randomSeed );


    // # ����� ������ �� ���� ���������.
    ListenerPlanet::mEngine = this;
}





inline Engine::~Engine() {
    // ����������� ����� ��������� ��� ������ ������� � OpenCL
    clReleaseMemObject( aboutPlanetCL );

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






inline void Engine::incarnate( std::shared_ptr< portulan_t >  p,  real_t extentPortulan ) {
    EngineWithoutBooster::incarnate( p, extentPortulan );

    // ���� ������ ������� �������������� ����������

    // �������������� ����. ��������� ��� ���� OpenCL
    // aboutPlanet
    assert( !mPortulan.expired() );
    aboutPlanetCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeAboutPlanet,
        &mPortulan.lock()->topology().topology().aboutPlanet,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // �������������� ���� OpenCL (���� ������� ����������)
    prepare();

}




inline Engine::real_t Engine::extent() {
    // ����� ����� �������������� ����� - ��������� ��������
    if (mExtent > 0.0) {
        return mExtent;
    }

    // ������������ ����� ������� �� ������� ��������� �������
    auto& topology = mPortulan.lock()->topology().topology();
    mExtent = topology.aboutPlanet.radius.atmosphere * 2.0f;

    return mExtent;
}





inline void Engine::pulse( int n ) {
    // ��������� 'n' ������
    for (int i = 0; i < n; ++i) {
        pulse();
    }
}






inline void Engine::pulse() {

    // @todo ...


    mLive.inc( mTimestep );


    // ����� �������
    notifyAfterPulse();
}





inline void Engine::prepareCLContext() {

    // @source http://nvidia.com/content/cuda/cuda-downloads.html / oclMarchingCubes.cpp

    //Get the platform
    errorCL = oclGetPlatformID( &platformCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // Get the number of devices available to the platform
#ifdef GPU_OPENCL_PLANET_PORTE
    errorCL = clGetDeviceIDs( platformCL, CL_DEVICE_TYPE_GPU, 0, nullptr, &devCountCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    devicesCL = new cl_device_id[ devCountCL ];
    errorCL = clGetDeviceIDs( platformCL, CL_DEVICE_TYPE_GPU, devCountCL, devicesCL, nullptr );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
#else
    errorCL = clGetDeviceIDs( platformCL, CL_DEVICE_TYPE_CPU, 0, nullptr, &devCountCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    devicesCL = new cl_device_id[ devCountCL ];
    errorCL = clGetDeviceIDs( platformCL, CL_DEVICE_TYPE_CPU, devCountCL, devicesCL, nullptr );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
#endif

    size_t endDevCL = devCountCL - 1;
    deviceUsedCL = CLAMP( deviceUsedCL, 0, endDevCL );
    endDevCL = deviceUsedCL;

    // No GL interop
    const cl_context_properties props[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformCL, 0 };
    // (!) ���� ���������� ������� ������� ������ �����, OpenCL �� ����� ���������������.
    //     ��������� LARGEADDRESSAWARE �� ������ ��������.
    gpuContextCL = clCreateContext(
        props, 1, &devicesCL[ deviceUsedCL ],
        &pfn_notify_cl, nullptr, &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << "������� ���������� OpenCL:" << std::endl;
    oclPrintDevInfo( LOGCONSOLE, devicesCL[ deviceUsedCL ] );
#endif

}





inline void Engine::prepareCLCommandQueue() {
    assert( gpuContextCL
        && "�������� OpenCL ��������� ���������������� �� ���������� ����� ������." );

    // create a command-queue
    commandQueueCL = clCreateCommandQueue( gpuContextCL, devicesCL[ deviceUsedCL ], 0, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}





template< size_t G >
inline void Engine::compileCLKernel(
    const std::vector< std::string >&  kernelKeys,
    const std::vector< std::string >&  includeHCL,
    const std::string& options
) {
    // # �������� � ������� ������ ���������������� � ������������.

#ifdef _DEBUG
        std::cout << std::endl << std::endl;
#endif

    typedef typelib::CubeSMC3D< G >  smc_t;

    // ������� ��� ������� ����� ������� �����, ����� �� ��������� ����
    // # ������� - ������� ������ �������� 1 ������ ������� ����� �
    //   ����� ����� ��������� �������� ��� ������.
    const float scale =
        mPortulan.lock()->topology().topology().aboutPlanet.size /
        static_cast< float >( G );
    const float partScale =
        scale / mPortulan.lock()->topology().topology().aboutPlanet.size;

    std::ostringstream commonOptions;
    commonOptions
        // ����� �������� ��� float
        << std::fixed
        // #! ������� ����� ������������� ��������: ���� ������
        //   � helper.hcl, ������� ���������� ��.
        << " -D GRID=" << G
        << " -D MIN_COORD_GRID=" << smc_t::minCoord().x
        << " -D MAX_COORD_GRID=" << smc_t::maxCoord().x
        << " -D SCALE=" << scale
        << " -D PART_SCALE=" << partScale
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
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/pragma.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/restruct.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/helper.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/structure.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/component.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/temperature.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/surface-temperature.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/rainfall.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/drainage.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/landscape.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/illuminance.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/biome.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/biome-set.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/living.h" )
        // ��� h-����� ���� ��������, �.�. ��� �������� �
        // planet.h (������������ �� ���� �����)
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/planet.h" )
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
            L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/" + fileKernel;
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
        cl_kernel oneKernelCL = clCreateKernel( programCL, kernelName.c_str(), &errorCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        kernelCL[ kernelKey ] = oneKernelCL;

        // ��������� ������� ���� ���������� ����� ������
        clReleaseProgram( programCL );

    } // for (auto itr

}





inline std::string Engine::commonConstantCLKernel() {
    // ��������� ��� ���������� ���������� ��������� ��� �����
    typedef typelib::CubeSMC3D< pnp::COMPONENT_GRID >    componentSMC_t;
    typedef typelib::CubeSMC3D< pnp::TEMPERATURE_GRID >  temperatureSMC_t;
    typedef typelib::CubeSMC3D< pnp::SURFACE_TEMPERATURE_GRID >  surfaceTemperatureSMC_t;
    typedef typelib::CubeSMC3D< pnp::RAINFALL_GRID >     rainfallSMC_t;
    typedef typelib::CubeSMC3D< pnp::DRAINAGE_GRID >     drainageSMC_t;
    typedef typelib::CubeSMC3D< pnp::LANDSCAPE_GRID >    landscapeSMC_t;
    typedef typelib::CubeSMC3D< pnp::ILLUMINANCE_GRID >  illuminanceSMC_t;
    typedef typelib::CubeSMC3D< pnp::BIOME_GRID >        biomeSMC_t;
    typedef typelib::CubeSMC3D< pnp::LIVING_GRID >       livingSMC_t;

    std::ostringstream options;
    options
        // ����� �������� ��� float
        << std::fixed

        // �� ��� ����� OpenCL (������� ������� ����)
#ifdef GPU_OPENCL_PLANET_PORTE
        << " -D GPU_OPENCL"
#else
        << " -D CPU_OPENCL"
#endif

        // @todo optimize fine ���������� ������ ����������� ���� ���������.

        // component
        << " -D COMPONENT_GRID=" << pnp::COMPONENT_GRID
        << " -D MIN_COORD_COMPONENT_GRID=" << componentSMC_t::minCoord().x
        << " -D MAX_COORD_COMPONENT_GRID=" << componentSMC_t::maxCoord().x
        << " -D COMPONENT_COUNT=" << pnp::COMPONENT_COUNT
        << " -D COMPONENT_CELL=" << pnp::COMPONENT_CELL

        // temperature
        << " -D TEMPERATURE_GRID=" << pnp::TEMPERATURE_GRID
        << " -D MIN_COORD_TEMPERATURE_GRID=" << temperatureSMC_t::minCoord().x
        << " -D MAX_COORD_TEMPERATURE_GRID=" << temperatureSMC_t::maxCoord().x

        // surfaceTemperature
        << " -D SURFACE_TEMPERATURE_GRID=" << pnp::SURFACE_TEMPERATURE_GRID
        << " -D MIN_COORD_SURFACE_TEMPERATURE_GRID=" << surfaceTemperatureSMC_t::minCoord().x
        << " -D MAX_COORD_SURFACE_TEMPERATURE_GRID=" << surfaceTemperatureSMC_t::maxCoord().x

        // rainfall
        << " -D RAINFALL_GRID=" << pnp::RAINFALL_GRID
        << " -D MIN_COORD_RAINFALL_GRID=" << rainfallSMC_t::minCoord().x
        << " -D MAX_COORD_RAINFALL_GRID=" << rainfallSMC_t::maxCoord().x

        // drainage
        << " -D DRAINAGE_GRID=" << pnp::DRAINAGE_GRID
        << " -D MIN_COORD_DRAINAGE_GRID=" << drainageSMC_t::minCoord().x
        << " -D MAX_COORD_DRAINAGE_GRID=" << drainageSMC_t::maxCoord().x

        // landscape
        << " -D LANDSCAPE_GRID=" << pnp::LANDSCAPE_GRID
        << " -D MIN_COORD_LANDSCAPE_GRID=" << landscapeSMC_t::minCoord().x
        << " -D MAX_COORD_LANDSCAPE_GRID=" << landscapeSMC_t::maxCoord().x
        << " -D LANDSCAPE_CELL=" << pnp::LANDSCAPE_CELL

        // illuminance
        << " -D ILLUMINANCE_GRID=" << pnp::ILLUMINANCE_GRID
        << " -D MIN_COORD_ILLUMINANCE_GRID=" << illuminanceSMC_t::minCoord().x
        << " -D MAX_COORD_ILLUMINANCE_GRID=" << illuminanceSMC_t::maxCoord().x
        << " -D ILLUMINANCE_STAR_COUNT=" << pnp::ILLUMINANCE_STAR_COUNT

        // biome
        << " -D BIOME_GRID=" << pnp::BIOME_GRID
        << " -D MIN_COORD_BIOME_GRID=" << biomeSMC_t::minCoord().x
        << " -D MAX_COORD_BIOME_GRID=" << biomeSMC_t::maxCoord().x
        << " -D BIOME_COUNT=" << pnp::BIOME_COUNT
        << " -D BIOME_CELL=" << pnp::BIOME_CELL
        << " -D LANDSCAPE_BIOME_COUNT=" << pnp::LANDSCAPE_BIOME_COUNT

        // living
        << " -D LIVING_GRID=" << pnp::LIVING_GRID
        << " -D MIN_COORD_LIVING_GRID=" << livingSMC_t::minCoord().x
        << " -D MAX_COORD_LIVING_GRID=" << livingSMC_t::maxCoord().x
        << " -D LIVING_COUNT=" << pnp::LIVING_COUNT
        << " -D LIVING_CELL=" << pnp::LIVING_CELL
        << " -D LIFE_CYCLE_COUNT=" << pnp::LIFE_CYCLE_COUNT
        << " -D PART_LIVING=" << pnp::PART_LIVING
        << " -D ATTACK_PART_LIVING=" << pnp::ATTACK_PART_LIVING
        << " -D RESIST_PART_LIVING=" << pnp::RESIST_PART_LIVING
        << " -D COMPONENT_COMPOSITION_LIVING=" << pnp::COMPONENT_COMPOSITION_LIVING
        << " -D COMPONENT_NEED_LIVING=" << pnp::COMPONENT_NEED_LIVING
        << " -D COMPONENT_WASTE_LIVING=" << pnp::COMPONENT_WASTE_LIVING
        << " -D ENERGY_NEED_LIVING=" << pnp::ENERGY_NEED_LIVING
        << " -D ENERGY_WASTE_LIVING=" << pnp::ENERGY_WASTE_LIVING
        << " -D HABITAT_SURVIVOR_LIVING=" << pnp::HABITAT_SURVIVOR_LIVING
        << " -D BIOME_COMFORT_SURVIVOR_LIVING=" << pnp::BIOME_COMFORT_SURVIVOR_LIVING
        << " -D FRIEND_COUNT_LIVING=" << pnp::FRIEND_COUNT_LIVING
        << " -D ENEMY_COUNT_LIVING=" << pnp::ENEMY_COUNT_LIVING

        // �������� ��������� �������� � ����. ������
        << " -D PRECISION=" << typelib::PRECISION

        // ��������� ��� �������� ��������
        << " -D IMMUNE=" << pnp::IMMUNE
        << " -D INFINITYf=" << pnp::INFINITYf
        << " -D ANYf=" << pnp::ANYf

        // ���������� ���������
        // ������� �� �������� � �������
        << " -D CK_TEMPERATURE=" << typelib::constant::physics::CK
        /* - @todo ...
        // ������� ����������, �� / (���� * K)
        // @source http://ru.wikipedia.org/wiki/%D0%A3%D0%BD%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B0%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D0%B3%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%8F_%D0%BF%D0%BE%D1%81%D1%82%D0%BE%D1%8F%D0%BD%D0%BD%D0%B0%D1%8F
        << " -D R_GAS=8.31441f"
        */

        << "";

    return options.str();
}





inline std::string Engine::commonOptionCLKernel() {
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






inline void Engine::fnErrorCL( int exitCode ) {
    std::cerr << "��� ������ OpenCL: " << exitCode << std::endl;

    // @todo fine ����������� ����������.
    std::cin.ignore();
    std::exit( exitCode );
}







inline void __stdcall pfn_notify_cl(
    const char* errinfo, const void* private_info,
    size_t cb, void* user_data
) {
    //fprintf( stderr, "\n(!) OpenCL error via pfn_notify_cl(): %s\n", errinfo );
    std::cout << std::endl << "(!) OpenCL error via pfn_notify_cl(): " << *errinfo << std::endl;
};



/*
inline void __stdcall pfn_notify_program_cl(
    cl_program, void* user_data
) {
    fprintf( stderr, "\n(!) OpenCL error via pfn_notify_program_cl().\n" );
};
*/


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
