namespace porte {


/**
* ����� ������� ������ DungeonCrawl.
*/


inline void DungeonCrawl::prepareCLKernel() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    prepareComponentCLKernel();
#endif

#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
    prepareTemperatureCLKernel();
#endif

#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
    prepareSurfaceTemperatureCLKernel();
#endif

#ifdef LIVING_DUNGEONCRAWL_PORTE
    prepareLivingCLKernel();
#endif
}





#ifdef COMPONENT_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareComponentCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;

    // �������������� ������� �����
    static const size_t grid = pd::COMPONENT_GRID;
    
    // ����������� ���� OpenCL
    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/temperature/top/init" )
    ;

    // �������������� ������� �����
    static const size_t grid = pd::TEMPERATURE_GRID;

    // ����������� ���� OpenCL
    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareSurfaceTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/surface-temperature/top/init" )
    ;

    // �������������� ������� �����
    static const size_t grid = pd::SURFACE_TEMPERATURE_GRID;

    // ����������� ���� OpenCL
    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareLivingCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/living/top/clear" )
        ( "scale/living/top/init" )
    ;

    // �������������� ������� �����
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

    // ����������� ���� OpenCL
    compileCLKernel< grid >( kernelKeys, options.str() );
}
#endif


} // porte
