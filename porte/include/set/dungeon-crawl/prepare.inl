namespace porte {


/**
* Часть методов класса DungeonCrawl.
*/


inline void DungeonCrawl::prepare() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    // @todo fine Убрать суффикс CLKernel.
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

    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    static const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;
    static const std::vector< std::string > includeHCL = boost::assign::list_of
        ( PATH_CL_DUNGEONCRAWL + "/include/dice.hcl" )
        ( PATH_CL_DUNGEONCRAWL + "/include/zone.hcl" )
    ;

    // Подготавливаем ядерные опции
    static const size_t grid = pd::COMPONENT_GRID;
    
    // Компилируем ядра OpenCL
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


} // porte
