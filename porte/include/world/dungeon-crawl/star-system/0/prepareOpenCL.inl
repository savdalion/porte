namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

/**
* ����� ������� ������ EngineHybrid.
*/


inline void EngineHybrid::prepare() {
    prepareEmitEvent();
}





inline void EngineHybrid::prepareEmitEvent() {
    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/emit-event/emit" )
        ( "set/emit-event/fix" )
    ;
    static const std::vector< std::string >  includeHCL;

    // ����������� ���� OpenCL
    compileCLKernel( kernelKeys, includeHCL );
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
