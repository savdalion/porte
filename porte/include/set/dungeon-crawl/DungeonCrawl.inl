namespace porte {


inline DungeonCrawl::DungeonCrawl(
    portulan_t* p
) :
    EngineWithoutBooster( p ),

    aboutPlanetCL( nullptr ),
    memsizeAboutPlanet( sizeof( portulan::planet::set::dungeoncrawl::aboutPlanet_t ) ),

    componentCL( nullptr ),
    // #! ���� ������ �������� �����������, �������� � ����������� ���������.
    memsizeComponent( sizeof( portulan::planet::set::dungeoncrawl::componentCell_t ) *
        portulan::planet::set::dungeoncrawl::COMPONENT_GRID *
        portulan::planet::set::dungeoncrawl::COMPONENT_GRID *
        portulan::planet::set::dungeoncrawl::COMPONENT_GRID
    ),

    temperatureCL( nullptr ),
    memsizeTemperature( sizeof( portulan::planet::set::dungeoncrawl::temperatureCell_t ) *
        portulan::planet::set::dungeoncrawl::TEMPERATURE_GRID *
        portulan::planet::set::dungeoncrawl::TEMPERATURE_GRID *
        portulan::planet::set::dungeoncrawl::TEMPERATURE_GRID
    ),

    surfaceTemperatureCL( nullptr ),
    memsizeSurfaceTemperature( sizeof( portulan::planet::set::dungeoncrawl::surfaceTemperatureCell_t ) *
        portulan::planet::set::dungeoncrawl::SURFACE_TEMPERATURE_GRID *
        portulan::planet::set::dungeoncrawl::SURFACE_TEMPERATURE_GRID *
        portulan::planet::set::dungeoncrawl::SURFACE_TEMPERATURE_GRID
    ),

    rainfallCL( nullptr ),
    memsizeRainfall( sizeof( portulan::planet::set::dungeoncrawl::rainfallCell_t ) *
        portulan::planet::set::dungeoncrawl::RAINFALL_GRID *
        portulan::planet::set::dungeoncrawl::RAINFALL_GRID *
        portulan::planet::set::dungeoncrawl::RAINFALL_GRID
    ),

    drainageCL( nullptr ),
    memsizeDrainage( sizeof( portulan::planet::set::dungeoncrawl::drainageCell_t ) *
        portulan::planet::set::dungeoncrawl::DRAINAGE_GRID *
        portulan::planet::set::dungeoncrawl::DRAINAGE_GRID *
        portulan::planet::set::dungeoncrawl::DRAINAGE_GRID
    ),

    biomeCL( nullptr ),
    memsizeBiome( sizeof( portulan::planet::set::dungeoncrawl::biomeCell_t ) *
        portulan::planet::set::dungeoncrawl::BIOME_GRID *
        portulan::planet::set::dungeoncrawl::BIOME_GRID *
        portulan::planet::set::dungeoncrawl::BIOME_GRID
    ),

    livingCL( nullptr ),
    memsizeLiving( sizeof( portulan::planet::set::dungeoncrawl::livingCell_t ) *
        portulan::planet::set::dungeoncrawl::LIVING_GRID *
        portulan::planet::set::dungeoncrawl::LIVING_GRID *
        portulan::planet::set::dungeoncrawl::LIVING_GRID
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
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
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

    // surfaceTemperature
    surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeSurfaceTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // rainfall
    rainfallCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeRainfall,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // drainage
    drainageCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeDrainage,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // biome
    biomeCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeBiome,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().biome.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // living
    livingCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLiving,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().living.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // �������������� ���� OpenCL (���� ������� ����������)
    prepareCLKernel();


    // ��������� ����� ��� ���������� ����. �����
    randomCore.seed( randomSeed );
}





inline DungeonCrawl::~DungeonCrawl() {
    /* - ���: ����� ��� OpenCL ������ �� ��������� ����� ���������...
    // ����������� ��������� ��� ������ ������� � OpenCL
    clReleaseMemObject( aboutPlanetCL );
    ...
    */
    // ...�� ��������� ��������� - ����������
    //clReleaseMemObject( workComponentCL );
    //clReleaseMemObject( workTemperatureCL );
    //...

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






inline void DungeonCrawl::pulse( int n ) {
    assert( false && "�� �����������." );
}






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





inline void DungeonCrawl::prepareCLCommandQueue() {
    assert( gpuContextCL
        && "�������� OpenCL ��������� ���������������� �� ���������� ����� ������." );

    // create a command-queue
    commandQueueCL = clCreateCommandQueue( gpuContextCL, devicesCL[ deviceUsedCL ], 0, &errorCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
}





template< size_t G >
inline void DungeonCrawl::compileCLKernel(
    const std::vector< std::string >&  kernelKeys,
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
        mPortulan->topology().topology().aboutPlanet.size /
        static_cast< float >( G );
    const float partScale =
        scale / mPortulan->topology().topology().aboutPlanet.size;

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
    // #! ����� ������� �����������.
    const std::vector< std::string > hcl = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/pragma.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/restruct.hcl" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/structure.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/component.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/temperature.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/surface-temperature.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/rainfall.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/drainage.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/biome.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/biome-set.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/living.h" )
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/planet.h" )
        ( PATH_CL_DUNGEONCRAWL + "/include/helper.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        // ������ ��� ������ �� �����������
        ( PATH_CL_DUNGEONCRAWL + "/include/biome.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;
    for (auto itr = hcl.cbegin(); itr != hcl.cend(); ++itr) {
        const std::string& pathAndName = *itr;
#ifdef _DEBUG
        const std::string shortName =
            itr->substr( pathAndName.find_last_of( '/' ) + 1 );
        std::cout << "�������� \"" << shortName << "\" .. ";
#endif
        const std::ifstream  file( pathAndName.c_str() );
        assert( file.is_open()
            && "���� �� ������." );
        std::stringstream buffer;
        buffer << file.rdbuf();
        kernelLibraryCode += "\n\n\n\n\n" + buffer.str();
#ifdef _DEBUG
    std::cout << " ��" << std::endl;
#endif
    }


    // ����������� ����
    // @todo ������ �� ������ � "scale". ������ - ������������� ���� �
    //       kernelKeys, �����.
    for (auto itr = std::begin( kernelKeys ); itr != std::end( kernelKeys ); ++itr) {
        //   # ��������� �� "/" �������� �������� ������ ����.
        const std::string kernelKey = *itr;
        const std::string kernelName = itr->substr( itr->find_last_of( '/' ) + 1 );

        // Program Setup
        const std::string fileKernel = kernelKey + ".cl";
        const std::string pathAndName =
            PATH_CL_DUNGEONCRAWL + "/" + fileKernel;
#ifdef _DEBUG
        std::cout << "�������� \"" << fileKernel << "\" .. ";
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
            kernelLibraryCode + "\n\n\n\n\n" + kernelSC;
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





inline std::string DungeonCrawl::commonConstantCLKernel() {
    namespace pd = portulan::planet::set::dungeoncrawl;

    // ��������� ��� ���������� ���������� ��������� ��� �����
    typedef typelib::CubeSMC3D< pd::COMPONENT_GRID >    componentSMC_t;
    typedef typelib::CubeSMC3D< pd::TEMPERATURE_GRID >  temperatureSMC_t;
    typedef typelib::CubeSMC3D< pd::SURFACE_TEMPERATURE_GRID >  surfaceTemperatureSMC_t;
    typedef typelib::CubeSMC3D< pd::RAINFALL_GRID >     rainfallSMC_t;
    typedef typelib::CubeSMC3D< pd::DRAINAGE_GRID >     drainageSMC_t;
    typedef typelib::CubeSMC3D< pd::BIOME_GRID >        biomeSMC_t;
    typedef typelib::CubeSMC3D< pd::LIVING_GRID >       livingSMC_t;

    std::ostringstream options;
    options
        // ����� �������� ��� float
        << std::fixed

        // component
        << " -D COMPONENT_GRID=" << pd::COMPONENT_GRID
        << " -D MIN_COORD_COMPONENT_GRID=" << componentSMC_t::minCoord().x
        << " -D MAX_COORD_COMPONENT_GRID=" << componentSMC_t::maxCoord().x
        << " -D COMPONENT_COUNT=" << pd::COMPONENT_COUNT
        << " -D COMPONENT_CELL=" << pd::COMPONENT_CELL

        // temperature
        << " -D TEMPERATURE_GRID=" << pd::TEMPERATURE_GRID
        << " -D MIN_COORD_TEMPERATURE_GRID=" << temperatureSMC_t::minCoord().x
        << " -D MAX_COORD_TEMPERATURE_GRID=" << temperatureSMC_t::maxCoord().x

        // surfaceTemperature
        << " -D SURFACE_TEMPERATURE_GRID=" << pd::SURFACE_TEMPERATURE_GRID
        << " -D MIN_COORD_SURFACE_TEMPERATURE_GRID=" << surfaceTemperatureSMC_t::minCoord().x
        << " -D MAX_COORD_SURFACE_TEMPERATURE_GRID=" << surfaceTemperatureSMC_t::maxCoord().x

        // rainfall
        << " -D RAINFALL_GRID=" << pd::RAINFALL_GRID
        << " -D MIN_COORD_RAINFALL_GRID=" << rainfallSMC_t::minCoord().x
        << " -D MAX_COORD_RAINFALL_GRID=" << rainfallSMC_t::maxCoord().x

        // drainage
        << " -D DRAINAGE_GRID=" << pd::DRAINAGE_GRID
        << " -D MIN_COORD_DRAINAGE_GRID=" << drainageSMC_t::minCoord().x
        << " -D MAX_COORD_DRAINAGE_GRID=" << drainageSMC_t::maxCoord().x

        // biome
        << " -D BIOME_GRID=" << pd::BIOME_GRID
        << " -D MIN_COORD_BIOME_GRID=" << biomeSMC_t::minCoord().x
        << " -D MAX_COORD_BIOME_GRID=" << biomeSMC_t::maxCoord().x
        << " -D BIOME_COUNT=" << pd::BIOME_COUNT
        << " -D BIOME_CELL=" << pd::BIOME_CELL

        // living
        << " -D LIVING_GRID=" << pd::LIVING_GRID
        << " -D MIN_COORD_LIVING_GRID=" << livingSMC_t::minCoord().x
        << " -D MAX_COORD_LIVING_GRID=" << livingSMC_t::maxCoord().x
        << " -D LIVING_COUNT=" << pd::LIVING_COUNT
        << " -D LIVING_CELL=" << pd::LIVING_CELL
        << " -D LIFE_CYCLE_COUNT=" << pd::LIFE_CYCLE_COUNT
        << " -D PART_LIVING=" << pd::PART_LIVING
        << " -D ATTACK_PART_LIVING=" << pd::ATTACK_PART_LIVING
        << " -D RESIST_PART_LIVING=" << pd::RESIST_PART_LIVING
        << " -D COMPONENT_COMPOSITION_LIVING=" << pd::COMPONENT_COMPOSITION_LIVING
        << " -D COMPONENT_NEED_LIVING=" << pd::COMPONENT_NEED_LIVING
        << " -D COMPONENT_WASTE_LIVING=" << pd::COMPONENT_WASTE_LIVING
        << " -D ENERGY_NEED_LIVING=" << pd::ENERGY_NEED_LIVING
        << " -D ENERGY_WASTE_LIVING=" << pd::ENERGY_WASTE_LIVING
        << " -D HABITAT_SURVIVOR_LIVING=" << pd::HABITAT_SURVIVOR_LIVING
        << " -D BIOME_COMFORT_SURVIVOR_LIVING=" << pd::BIOME_COMFORT_SURVIVOR_LIVING

        // �������� ��������� �������� � ����. ������
        << " -D PRECISION=" << typelib::PRECISION

        // ��������� ��� �������� ��������
        << " -D IMMUNE=" << pd::IMMUNE
        << " -D INFINITYf=" << pd::INFINITYf
        << " -D ANYf=" << pd::ANYf

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





inline std::string DungeonCrawl::commonOptionCLKernel() {
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
        // (i) �������� ������� ������ ��� ������������ ����: ����� ���
        //     20-������� ����������.
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
