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
        ( "set/emit-event/asteroid/begin" )
        ( "set/emit-event/asteroid/direct" )
        ( "set/emit-event/asteroid/relative" )
        ( "set/emit-event/asteroid/fix" )
        ( "set/emit-event/star/begin" )
        ( "set/emit-event/star/direct" )
        ( "set/emit-event/star/relative" )
        ( "set/emit-event/star/fix" )
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
