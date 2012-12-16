namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {

/**
* Часть методов класса Engine.
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

    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/component/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    // Подготавливаем ядерные опции
    static const size_t grid = pnp::COMPONENT_GRID;
    
    // Компилируем ядра OpenCL
    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void Engine::prepareTemperature() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/temperature/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void Engine::prepareSurfaceTemperature() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/surface-temperature/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::SURFACE_TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef RAINFALL_DUNGEONCRAWL_PORTE
inline void Engine::prepareRainfall() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/rainfall/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::RAINFALL_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
inline void Engine::prepareDrainage() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/drainage/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::DRAINAGE_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
inline void Engine::prepareLandscape() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/landscape/clear" )
        ( "set/landscape/initA" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/landscape.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::LANDSCAPE_GRID;
    
    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef BIOME_DUNGEONCRAWL_PORTE
inline void Engine::prepareBiome() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/biome/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN + "/set/biome-utils.h" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/biome.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::BIOME_GRID;

    compileCLKernel< grid >( kernelKeys, includeHCL );
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void Engine::prepareLiving() {

    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "set/living/clear" )
        ( "set/living/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/dice.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/biome.hcl" )
        ( L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE + "/set/zone.hcl" )
    ;

    static const size_t grid = pnp::LIVING_GRID;

    std::ostringstream options;
    options
        // лечим точность для float
        << std::fixed

        // Для метода init(), определяют как быстро будет
        // увеличиваться кол-во особей в 1-й ячейке.
        // Коэф. прямо пропорционален желаемому кол-ву из zoneLiving_t.
        // (малые значения - быстрее вычисления, но более грубое
        // расселение особей)
        << " -D ITERATION_GROW_COUNT=" << 10

        // При включении:
        // Наличие в соседних ячейках подобных особей повышает вероятность
        // увеличения кол-ва особей в текущей ячейке.
        // #! Т.к. расселение особей ведётся без промежуточной матрицы
        //    и в ядре OpenCL отсутствует возможность синхронизовать потоки
        //    на глобальном уровне, возможна неточность при подсчёте кол-ва
        //    соседей. Здесь ошибка в кол-ве - вполне приемлема. Но надо
        //    помнить, что в общем случае она влечёт разное наполнение
        //    матриц даже при одинаковом "seed" (см. __kernel).
        //<< " -D WITH_NEIGHBOUR_EXPANSION"

        // При включении:
        // Наличие в текущей ячейке подобных особей повышает вероятность
        // увеличения кол-ва особей в этой же ячейке.
        // Матрица, в отличии от WITH_NEIGHBOUR_EXPANSION, стабильна.
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
