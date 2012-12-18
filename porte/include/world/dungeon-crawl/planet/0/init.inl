namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {

/**
* Часть методов класса Engine.
*/


inline void Engine::init() {
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    initComponent();
#endif

#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
    initTemperature();
#endif

#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
    initSurfaceTemperature();
#endif

#ifdef RAINFALL_DUNGEONCRAWL_PORTE
    initRainfall();
#endif

#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
    initDrainage();
#endif

#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
    initLandscape();
#endif

#ifdef ILLUMINANCE_DUNGEONCRAWL_PORTE
    initIlluminance();
#endif

#ifdef BIOME_DUNGEONCRAWL_PORTE
    initBiome();
#endif

#ifdef LIVING_DUNGEONCRAWL_PORTE
    initLiving();
#endif
}





#ifdef COMPONENT_DUNGEONCRAWL_PORTE
inline void Engine::initComponent() {

#ifdef _DEBUG
    std::cout << "Определяем состав планеты ..";
#endif

    const cl_mem componentCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeComponent,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::COMPONENT_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // Инициализируем сетку
    const cl_kernel kernel = kernelCL[ "set/component/init" ];

    errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 1, sizeof( const cl_mem ), &componentCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        componentCL,
        CL_TRUE,
        0,
        memsizeComponent,
        mPortulan->topology().topology().component.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( componentCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void Engine::initTemperature() {

#ifdef _DEBUG
    std::cout << "Разогреваем планету ..";
#endif

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::TEMPERATURE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // Задаём температуру планетной области
    const cl_kernel kernelInit = kernelCL[ "set/temperature/init" ];

    /* @test
    pnp::test_t test;
    const size_t memsizeTest = sizeof( pnp::test_t );
    const cl_mem testCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeTest,
        &test,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &testCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    */


    errorCL = clSetKernelArg( kernelInit, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 1, sizeof( const cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInit,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        temperatureCL,
        CL_TRUE,
        0,
        memsizeTemperature,
        mPortulan->topology().topology().temperature.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( temperatureCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void Engine::initSurfaceTemperature() {

#ifdef _DEBUG
    std::cout << "Рассчитываем температуру поверхности ..";
#endif

    const cl_mem surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeSurfaceTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    static const size_t grid = pnp::SURFACE_TEMPERATURE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // Задаём температуру на поверхности планеты
    const cl_kernel kernelInit = kernelCL[ "set/surface-temperature/init" ];

    errorCL = clSetKernelArg( kernelInit, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 1, sizeof( const cl_mem ), &surfaceTemperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInit,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        surfaceTemperatureCL,
        CL_TRUE,
        0,
        memsizeSurfaceTemperature,
        mPortulan->topology().topology().surfaceTemperature.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( surfaceTemperatureCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef RAINFALL_DUNGEONCRAWL_PORTE
inline void Engine::initRainfall() {

#ifdef _DEBUG
    std::cout << "Оцениваем атмосферные осадки ..";
#endif

    const cl_mem rainfallCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeRainfall,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::RAINFALL_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // Задаём атм. осадки на поверхности планеты
    const cl_kernel kernelInit = kernelCL[ "set/rainfall/init" ];

    errorCL = clSetKernelArg( kernelInit, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 1, sizeof( const cl_mem ), &rainfallCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernelInit, 2, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInit,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        rainfallCL,
        CL_TRUE,
        0,
        memsizeRainfall,
        mPortulan->topology().topology().rainfall.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( rainfallCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
inline void Engine::initDrainage() {

#ifdef _DEBUG
    std::cout << "Генерируем дренаж ..";
#endif

    const cl_mem drainageCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeDrainage,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::DRAINAGE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };


    // Задаём дренаж на поверхности планеты
    const cl_kernel kernelInit = kernelCL[ "set/drainage/init" ];

    errorCL = clSetKernelArg( kernelInit, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 1, sizeof( const cl_mem ), &drainageCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernelInit, 2, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInit,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        drainageCL,
        CL_TRUE,
        0,
        memsizeDrainage,
        mPortulan->topology().topology().drainage.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( drainageCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
inline void Engine::initLandscape() {

#ifdef _DEBUG
    std::cout << "Задаём ландшафты ..";
#endif

    const cl_mem landscapeCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLandscape,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().landscape.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem componentCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeComponent,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeSurfaceTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem rainfallCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeRainfall,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem drainageCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeDrainage,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::LANDSCAPE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };


    // Очищаем матрицу
    cl_kernel kernelClear = kernelCL[ "set/landscape/clear" ];

    errorCL = clSetKernelArg( kernelClear, 0, sizeof( const cl_mem ), &landscapeCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelClear,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << ".";
#endif


    // Проход A
    // # Все составляющие ландшафтов должны быть предварительно очищены -
    //   см. kernel clear().
    const cl_kernel kernelInitA = kernelCL[ "set/landscape/initA" ];

    errorCL = clSetKernelArg( kernelInitA, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInitA, 1, sizeof( const cl_mem ), &landscapeCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInitA, 2, sizeof( const cl_mem ), &componentCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInitA, 3, sizeof( const cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInitA, 4, sizeof( const cl_mem ), &surfaceTemperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInitA, 5, sizeof( const cl_mem ), &rainfallCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInitA, 6, sizeof( const cl_mem ), &drainageCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernelInitA, 7, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInitA,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        landscapeCL,
        CL_TRUE,
        0,
        memsizeLandscape,
        mPortulan->topology().topology().landscape.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    // Проход B
    // @todo ...


    clReleaseMemObject( landscapeCL );
    clReleaseMemObject( componentCL );
    clReleaseMemObject( temperatureCL );
    clReleaseMemObject( surfaceTemperatureCL );
    clReleaseMemObject( rainfallCL );
    clReleaseMemObject( drainageCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef ILLUMINANCE_DUNGEONCRAWL_PORTE
inline void Engine::initIlluminance() {

#ifdef _DEBUG
    std::cout << "Освещаем планету ..";
#endif

    const cl_mem illuminanceCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeIlluminance,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().illuminance.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::ILLUMINANCE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // Задаём освещённость планетной области
    const cl_kernel kernelInit = kernelCL[ "set/illuminance/init" ];

    errorCL = clSetKernelArg( kernelInit, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 1, sizeof( const cl_mem ), &illuminanceCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInit,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        illuminanceCL,
        CL_TRUE,
        0,
        memsizeIlluminance,
        mPortulan->topology().topology().illuminance.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( illuminanceCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef BIOME_DUNGEONCRAWL_PORTE
inline void Engine::initBiome() {

#ifdef _DEBUG
    std::cout << "Распознаём биомы ..";
#endif

    const cl_mem biomeCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeBiome,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().biome.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeSurfaceTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem rainfallCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeRainfall,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem drainageCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeDrainage,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem landscapeCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeLandscape,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().landscape.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::BIOME_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };


    // Распознаём биомы
    // # Все составляющие биомов должны быть предварительно инициализированы.
    const cl_kernel kernelInit = kernelCL[ "set/biome/init" ];

    errorCL = clSetKernelArg( kernelInit, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 1, sizeof( const cl_mem ), &biomeCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 2, sizeof( const cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 3, sizeof( const cl_mem ), &surfaceTemperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 4, sizeof( const cl_mem ), &rainfallCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 5, sizeof( const cl_mem ), &drainageCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernelInit, 6, sizeof( const cl_mem ), &landscapeCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernelInit, 7, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernelInit,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // синхронизация
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // результат
    errorCL = clEnqueueReadBuffer(
        commandQueueCL,
        biomeCL,
        CL_TRUE,
        0,
        memsizeBiome,
        mPortulan->topology().topology().biome.content,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    clReleaseMemObject( biomeCL );
    clReleaseMemObject( temperatureCL );
    clReleaseMemObject( surfaceTemperatureCL );
    clReleaseMemObject( rainfallCL );
    clReleaseMemObject( drainageCL );
    clReleaseMemObject( landscapeCL );

#ifdef _DEBUG
    std::cout << " ОК" << std::endl;
#endif
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void Engine::initLiving() {
    // #! Структуры для передачи OpenCL должны быть подготовлены в prepareComponent().

#ifdef _DEBUG
    std::cout << "Населяем планету ..";
#endif

    const cl_mem livingCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLiving,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().living.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem componentCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeComponent,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeTemperature,
        // #! Если память выделена динамически, обращаемся к содержанию.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pnp::LIVING_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    auto& tp = mPortulan->topology().topology();

    // Инициализируем сетку

    // очищаем матрицу количеств
    cl_kernel kernel = kernelCL[ "set/living/clear" ];

    errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &livingCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clEnqueueNDRangeKernel(
        commandQueueCL,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE,
        nullptr,
        0, nullptr, nullptr
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    /** - Синхронизируем поток в начале цикла (см. ниже).
          Благодаря параллельной работе CPU и GPU это позволит
          ускорить выполнение кода.
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    */

#ifdef _DEBUG
    std::cout << ".";
#endif


    // вызываем init() до тех пор, пока кол-во особей на планете
    // не будет близко к желаемому
    kernel = kernelCL[ "set/living/init" ];

    errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 1, sizeof( const cl_mem ), &livingCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // На сколько мы приблизились к расселению заданного кол-ва особей.
    // подготавливаем проверочный аргумент
    pnp::zoneLivingCountComplete_t zoneCountComplete;
    static const size_t memsizeZoneCountComplete =
        sizeof( pnp::zoneOneLivingCountComplete_t ) *
        pnp::LIVING_COUNT * pnp::LIFE_CYCLE_COUNT;
    const cl_mem zoneCountCompleteCL = clCreateBuffer(
        gpuContextCL,
        // доп. память не выделяется
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeZoneCountComplete,
        &zoneCountComplete,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 2, sizeof( const cl_mem ), &zoneCountCompleteCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // # Заселять мир будем исключительно взрослыми особями.
    const pnp::LIFE_CYCLE lifeCycle = pnp::LC_ADULT;
    errorCL = clSetKernelArg( kernel, 3, sizeof( pnp::LIFE_CYCLE ), &lifeCycle );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 4, sizeof( const cl_mem ), &componentCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 5, sizeof( const cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // Нет смысла добавлять другое значение при каждом вызове ядра, чтобы
    // разнообразить мир: в ядре OpenCL предостаточно "магических чисел".
    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernel, 6, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    const auto& al = tp.aboutPlanet.living;

    // желаемое кол-во особей
    float allZoneDesireCount = 0.0f;
    pnp::zoneLivingCount_t zoneDesireCount = {};
    /* - Проще копировать код. См. ниже.
    const auto fnDesireCount = [ &zoneDesireCount, lifeCycle ] (
        float& zolValue,
        const pnp::zoneLiving_t& zl
    ) -> void {
        const pnp::CODE_LIVING code = zl.code;
        if (code != pnp::CL_NONE) {
            const float q = zl.count;
            zolValue += q;
            zoneDesireCount[ code ][ lifeCycle ].all += q;
        }
    };
    */
    for (size_t k = 0; k < pnp::LIVING_CELL; ++k) {
        // space
        {
            const auto& zk = al.space[k];
            const pnp::CODE_LIVING code = zk.code;
            if (code != pnp::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].space += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // atmosphere
        {
            const auto& zk = al.atmosphere[k];
            const pnp::CODE_LIVING code = zk.code;
            if (code != pnp::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].atmosphere += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // crust
        {
            const auto& zk = al.crust[k];
            const pnp::CODE_LIVING code = zk.code;
            if (code != pnp::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].crust += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // mantle
        {
            const auto& zk = al.mantle[k];
            const pnp::CODE_LIVING code = zk.code;
            if (code != pnp::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].mantle += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // core
        {
            const auto& zk = al.core[k];
            const pnp::CODE_LIVING code = zk.code;
            if (code != pnp::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].core += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
    } // for (size_t k


    // вызываем init(), анализируем полученное кол-во
    // @see Параметры для init().
    size_t iteration = 0;
    for ( ; ; ) {
        // синхронизируем потоки
        // (см. выше прим. после вызова ядра clear())
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // результат
        // первый вызов - проинициализированное выше нулями содержание,
        // последующие - результат работы ядра init()
        // @todo optimize Попробовать использовать livingCL без
        //       закачки данных в tp.living.content.
        errorCL = clEnqueueReadBuffer(
            commandQueueCL,
            livingCL,
            CL_TRUE,
            0,
            memsizeLiving,
            tp.living.content,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // подсчитываем кол-ва
        float allZoneCount = 0.0f;
        pnp::zoneLivingCount_t zoneCount = {};
        // пробежим по всем ячейкам
        static const size_t LG = pnp::LIVING_GRID * pnp::LIVING_GRID * pnp::LIVING_GRID;
        for (size_t i = 0; i < LG; ++i) {
            // в каждой ячейке содержится не более LIVING_CELL особей
            for (size_t k = 0; k < pnp::LIVING_CELL; ++k) {
                // информация об особях сгруппирована по LIFE_CYCLE
                for (size_t l = static_cast< size_t >( pnp::LC_EMBRYO);
                     l < static_cast< size_t >( pnp::LC_last );  ++l
                ) {
                    const pnp::portionLiving_t& p = tp.living.content[i][k][l];
                    const pnp::CODE_LIVING code = p.code;
                    assert( ((code >= pnp::CL_NONE) && (code < pnp::CL_last))
                        && "Код особи не распознан. Вероятно, ошибка при инициализации матрицы." );
                    // суммируем (инициализация была выше)
                    zoneCount[ code ][ l ].all += p.count;
                    allZoneCount += p.count;
                }
            }
        }

        // кол-во особей должно быть *близко* к желаемому
        std::memset( zoneCountComplete, false, memsizeZoneCountComplete );
        bool allComplete = true;
        for (size_t code = 0; code < pnp::LIVING_COUNT; ++code) {
            for (size_t l = static_cast< size_t >( pnp::LC_EMBRYO);
                 l < static_cast< size_t >( pnp::LC_last );  ++l
            ) {
                const float desireCount = zoneDesireCount[ code ][ l ].all;
                const float count       = zoneCount[ code ][ l ].all;
                // # Не используем группировку по зонам.
                auto& zcc = zoneCountComplete[ code ][ l ];
                zcc.all = (desireCount <= count * 1.1f);
                if ( !zcc.all ) {
                    allComplete = false;
                    break;
                }
            }
            if ( !allComplete ) {
                // true уже не будет
                break;
            }

        } // for (size_t code

        if ( allComplete ) {
            // планета заселена, количества особей *приближено* к желаемому
            break;
        }

        errorCL = clEnqueueNDRangeKernel(
            commandQueueCL,
            kernel,
            GRID_WORK_DIM,
            nullptr,
            GRID_GLOBAL_WORK_SIZE,
            nullptr,
            0, nullptr, nullptr
        );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // синхронизация
        /* - (синхронизируем в начале цикла)
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        */

        // результат
        // (получаем в начале цикла)

        ++iteration;

#ifdef _DEBUG
        //std::cout << ".";
        // человечный индикатор прогресса
        static const size_t W = 50;
        static const size_t PRECISION_DIGIT = 0;
        static const float PRECISION_POW =
            boost::math::pow< PRECISION_DIGIT >( 10 );
        if (iteration % W == 0) {
            const float percent = std::floor(
                allZoneCount / allZoneDesireCount * 100.0f * PRECISION_POW
            ) / PRECISION_POW;
            std::ostringstream ss;
            ss << std::fixed << std::setprecision( PRECISION_DIGIT ) <<
                " " << percent << " %";
            std::cout << ss.str() <<
                std::string( ss.str().length(), '\b');
        }
#endif
    } // for ( ; ; )


    clReleaseMemObject( livingCL );
    clReleaseMemObject( componentCL );
    clReleaseMemObject( temperatureCL );

#ifdef _DEBUG
    std::cout << " ОК          " << std::endl;
#endif
}
#endif


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
