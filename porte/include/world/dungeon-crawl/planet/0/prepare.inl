namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {

/**
* ����� ������� ������ Engine.
*/


inline void Engine::prepare() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    prepareComponent();
#endif

#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
    prepareTemperature();
#endif

#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
    prepareSurfaceTemperature();
#endif

#ifdef RAINFALL_DUNGEONCRAWL_PORTE
    prepareRainfall();
#endif

#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
    prepareDrainage();
#endif

#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
    prepareLandscape();
#endif

#ifdef BIOME_DUNGEONCRAWL_PORTE
    prepareBiome();
#endif

#ifdef LIVING_DUNGEONCRAWL_PORTE
    prepareLiving();
#endif
}





#ifdef COMPONENT_DUNGEONCRAWL_PORTE
inline void Engine::prepareComponent() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/component/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    // �������������� ������� �����
    static const size_t grid = pd::COMPONENT_GRID;
    
    // ����������� ���� OpenCL
    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void Engine::prepareTemperature() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/temperature/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void Engine::prepareSurfaceTemperature() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/surface-temperature/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::SURFACE_TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef RAINFALL_DUNGEONCRAWL_PORTE
inline void Engine::prepareRainfall() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/rainfall/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::RAINFALL_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
inline void Engine::prepareDrainage() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/drainage/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::DRAINAGE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
inline void Engine::prepareLandscape() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/landscape/clear" )
        ( "set/landscape/initA" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/landscape.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::LANDSCAPE_GRID;
    
    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef BIOME_DUNGEONCRAWL_PORTE
inline void Engine::prepareBiome() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/biome/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/biome-utils.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/biome.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::BIOME_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void Engine::prepareLiving() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/living/clear" )
        ( "set/living/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/biome.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pd::LIVING_GRID;

    std::ostringstream options;
    options
        // ����� �������� ��� float
        << std::fixed

        // ��� ������ init(), ���������� ��� ������ �����
        // ������������� ���-�� ������ � 1-� ������.
        // ����. ����� �������������� ��������� ���-�� �� zoneLiving_t.
        // (����� �������� - ������� ����������, �� ����� ������
        // ���������� ������)
        << " -D ITERATION_GROW_COUNT=" << 10

        // ��� ���������:
        // ������� � �������� ������� �������� ������ �������� �����������
        // ���������� ���-�� ������ � ������� ������.
        // #! �.�. ���������� ������ ������ ��� ������������� �������
        //    � � ���� OpenCL ����������� ����������� �������������� ������
        //    �� ���������� ������, �������� ���������� ��� �������� ���-��
        //    �������. ����� ������ � ���-�� - ������ ���������. �� ����
        //    �������, ��� � ����� ������ ��� ������ ������ ����������
        //    ������ ���� ��� ���������� "seed" (��. __kernel).
        //<< " -D WITH_NEIGHBOUR_EXPANSION"

        // ��� ���������:
        // ������� � ������� ������ �������� ������ �������� �����������
        // ���������� ���-�� ������ � ���� �� ������.
        // �������, � ������� �� WITH_NEIGHBOUR_EXPANSION, ���������.
        << " -D WITH_SELF_EXPANSION"

        << "";

    compileCLKernel< grid >( kernelKeys, includeHCL, options.str() );
}
#endif


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte