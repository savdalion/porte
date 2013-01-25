namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

/**
* ����� ������� ������ EngineHybrid.
*/


inline void EngineHybrid::prepare() {
    prepareEmitEvent( "asteroid" );
    //prepareEmitEvent( "planet" );
    //prepareEmitEvent( "star" );
}




inline void EngineHybrid::prepareEmitEvent( const std::string& element ) {
    // # �������� � ������� ������ ���������������� � ������������.

    // @todo fine ������ �� ������ � "scale". ������ - ������������� ����.
    std::vector< std::string > kernelKeys = boost::assign::list_of
        //( "set/emit-event/" + element + "/begin" )
        ( "set/emit-event/" + element + "/direct" )
        //( "set/emit-event/" + element + "/relative" )
        //( "set/emit-event/" + element + "/fix" )
    ;

    // ������� ������ ���������, ������� *�����* ���� �������
    // ����������� ���������� ���������
    const std::string pathPrefix = "set/unique-emit-event/" + element;
    std::set< std::string >  fl;
    typelib::file::listFolder(
        &fl,  L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE + "/" + pathPrefix
    );
    for (auto itr = fl.cbegin(); itr != fl.cend(); ++itr) {
        kernelKeys.push_back( pathPrefix + "/" + *itr + "/direct" );
    }

    const std::vector< std::string >  includeHCL;

    // ����������� ���� OpenCL
    compileCLKernel( kernelKeys, includeHCL );
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
