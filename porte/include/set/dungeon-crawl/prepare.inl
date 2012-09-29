namespace porte {


/**
* Часть методов класса DungeonCrawl.
*/


inline void DungeonCrawl::prepareCLKernel() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    // @todo fine Убрать суффикс CLKernel.
    prepareComponentCLKernel();
#endif

#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
    prepareTemperatureCLKernel();
#endif

#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
    prepareSurfaceTemperatureCLKernel();
#endif

#ifdef RAINFALL_DUNGEONCRAWL_PORTE
    prepareRainfallCLKernel();
#endif

#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
    prepareDrainageCLKernel();
#endif

#ifdef BIOME_DUNGEONCRAWL_PORTE
    prepareBiomeCLKernel();
#endif

#ifdef LIVING_DUNGEONCRAWL_PORTE
    prepareLivingCLKernel();
#endif
}





#ifdef COMPONENT_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareComponentCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    // # Контекст и очередь команд инициализированы в конструкторе.

    // @todo fine Искать по папкам в "scale". Сейчас - фиксированный путь.
    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/component/top/init" )
    ;

    // Подготавливаем ядерные опции
    static const size_t grid = pd::COMPONENT_GRID;
    
    // Компилируем ядра OpenCL
    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/temperature/top/init" )
    ;

    static const size_t grid = pd::TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareSurfaceTemperatureCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/surface-temperature/top/init" )
    ;

    static const size_t grid = pd::SURFACE_TEMPERATURE_GRID;

    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef RAINFALL_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareRainfallCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/rainfall/top/init" )
    ;

    static const size_t grid = pd::RAINFALL_GRID;

    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareDrainageCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/drainage/top/init" )
    ;

    static const size_t grid = pd::DRAINAGE_GRID;

    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef BIOME_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareBiomeCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/biome/top/init" )
    ;

    static const size_t grid = pd::BIOME_GRID;

    compileCLKernel< grid >( kernelKeys );
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::prepareLivingCLKernel() {

    namespace pd = portulan::planet::set::dungeoncrawl;

    const std::vector< std::string > kernelKeys = boost::assign::list_of
        ( "scale/living/top/clear" )
        ( "scale/living/top/init" )
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

    compileCLKernel< grid >( kernelKeys, options.str() );
}
#endif


} // porte
