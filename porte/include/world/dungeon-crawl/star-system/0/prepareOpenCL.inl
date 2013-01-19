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
        ( "set/emit-event/emit" )
        ( "set/emit-event/fix" )
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
