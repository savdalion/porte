namespace porte {


/**
* ����� ������� ������ DungeonCrawl.
*/


inline void DungeonCrawl::prepare() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    // @todo fine ������ ������� CLKernel.
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
inline void DungeonCrawl::prepareComponent() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    // �������������� ������� �����
    static const size_t grid = pd::COMPONENT_GRID;
    
    // ����������� ���� OpenCL
    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareTemperature() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/temperature/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    static const size_t grid = pd::TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareSurfaceTemperature() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/surface-temperature/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    static const size_t grid = pd::SURFACE_TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef RAINFALL_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareRainfall() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/rainfall/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    static const size_t grid = pd::RAINFALL_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareDrainage() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/drainage/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    static const size_t grid = pd::DRAINAGE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareLandscape() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/landscape/top/clear" )
        ( "scale/landscape/top/initA" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/landscape.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    static const size_t grid = pd::LANDSCAPE_GRID;
    
    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef BIOME_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareBiome() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/biome/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_STRUCTURE_CL_DUNGEONCRAWL + "/biome-utils.h" )
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/biome.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    static const size_t grid = pd::BIOME_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareLiving() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/living/top/clear" )
        ( "scale/living/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/biome.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
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


} // porte
