namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

/**
* ����� ������� ������ EngineOpenCL.
*/


inline void EngineOpenCL::prepare() {
    prepareGravity();
}





inline void EngineOpenCL::prepareGravity() {
    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/gravity/pulseA" )
        ( "set/gravity/fixA" )
    ;
    static const std::vector< std::string > includeHCL;

    // ����������� ���� OpenCL
    compileCLKernel( kernelKeys, includeHCL );
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
