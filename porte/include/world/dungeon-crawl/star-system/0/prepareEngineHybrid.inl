namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

/**
* Часть методов класса EngineHybrid.
*/


inline void EngineHybrid::prepare() {
    prepareEmitEvent();
}





inline void EngineHybrid::prepareEmitEvent() {
    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
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

    // Компилируем ядра OpenCL
    compileCLKernel( kernelKeys, includeHCL );
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
