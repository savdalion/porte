namespace porte {


/**
* ����� ������� ������ DungeonCrawl.
*/


inline void DungeonCrawl::init() {
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

#ifdef BIOME_DUNGEONCRAWL_PORTE
    initBiome();
#endif

#ifdef LIVING_DUNGEONCRAWL_PORTE
    initLiving();
#endif
}





#ifdef COMPONENT_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initComponent() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "���������� ������ ������� ..";
#endif

    const cl_mem componentCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeComponent,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::COMPONENT_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // �������������� �����
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    std::cout << " ��" << std::endl;
#endif
}
#endif





#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initTemperature() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "����������� ������� ..";
#endif

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::TEMPERATURE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // ����� ����������� ��������� �������
    const cl_kernel kernelInit = kernelCL[ "set/temperature/init" ];

    /* @test
    pd::test_t test;
    const size_t memsizeTest = sizeof( pd::test_t );
    const cl_mem testCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    std::cout << " ��" << std::endl;
#endif
}
#endif





#ifdef SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initSurfaceTemperature() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "������������ ����������� ����������� ..";
#endif

    const cl_mem surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeSurfaceTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    static const size_t grid = pd::SURFACE_TEMPERATURE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // ����� ����������� �� ����������� �������
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    std::cout << " ��" << std::endl;
#endif
}
#endif





#ifdef RAINFALL_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initRainfall() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "��������� ����������� ������ ..";
#endif

    const cl_mem rainfallCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeRainfall,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::RAINFALL_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    // ����� ���. ������ �� ����������� �������
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    std::cout << " ��" << std::endl;
#endif
}
#endif





#ifdef DRAINAGE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initDrainage() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "���������� ������ ..";
#endif

    const cl_mem drainageCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeDrainage,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::DRAINAGE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };


    // ����� ������ �� ����������� �������
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    std::cout << " ��" << std::endl;
#endif
}
#endif





#ifdef LANDSCAPE_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initLandscape() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "����� ��������� ..";
#endif

    const cl_mem landscapeCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLandscape,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().landscape.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem componentCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeComponent,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeSurfaceTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem rainfallCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeRainfall,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem drainageCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeDrainage,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::LANDSCAPE_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };


    // ������� �������
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


#ifdef _DEBUG
    std::cout << ".";
#endif


    // ������ A
    // # ��� ������������ ���������� ������ ���� �������������� ������� -
    //   ��. kernel clear().
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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


    // ������ B
    // @todo ...


    clReleaseMemObject( landscapeCL );
    clReleaseMemObject( componentCL );
    clReleaseMemObject( temperatureCL );
    clReleaseMemObject( surfaceTemperatureCL );
    clReleaseMemObject( rainfallCL );
    clReleaseMemObject( drainageCL );

#ifdef _DEBUG
    std::cout << " ��" << std::endl;
#endif
}
#endif




#ifdef BIOME_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initBiome() {

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "��������� ����� ..";
#endif

    const cl_mem biomeCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeBiome,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().biome.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem surfaceTemperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeSurfaceTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().surfaceTemperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem rainfallCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeRainfall,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().rainfall.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem drainageCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeDrainage,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().drainage.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem landscapeCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeLandscape,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().landscape.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::BIOME_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };


    // ��������� �����
    // # ��� ������������ ������ ������ ���� �������������� ����������������.
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

    // �������������
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ���������
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
    std::cout << " ��" << std::endl;
#endif
}
#endif





#ifdef LIVING_DUNGEONCRAWL_PORTE
inline void DungeonCrawl::initLiving() {
    // #! ��������� ��� �������� OpenCL ������ ���� ������������ � prepareComponent().

    namespace pd = portulan::world::dungeoncrawl::planet::l0;

#ifdef _DEBUG
    std::cout << "�������� ������� ..";
#endif

    const cl_mem livingCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        memsizeLiving,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().living.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem componentCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeComponent,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().component.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    const cl_mem temperatureCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeTemperature,
        // #! ���� ������ �������� �����������, ���������� � ����������.
        mPortulan->topology().topology().temperature.content,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    static const size_t grid = pd::LIVING_GRID;

    static const size_t GRID_WORK_DIM = 3;
    static const size_t GRID_GLOBAL_WORK_SIZE[] = { grid, grid, grid };

    auto& tp = mPortulan->topology().topology();

    // �������������� �����

    // ������� ������� ���������
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

    /** - �������������� ����� � ������ ����� (��. ����).
          ��������� ������������ ������ CPU � GPU ��� ��������
          �������� ���������� ����.
    errorCL = clFinish( commandQueueCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
    */

#ifdef _DEBUG
    std::cout << ".";
#endif


    // �������� init() �� ��� ���, ���� ���-�� ������ �� �������
    // �� ����� ������ � ���������
    kernel = kernelCL[ "set/living/init" ];

    errorCL = clSetKernelArg( kernel, 0, sizeof( const cl_mem ), &aboutPlanetCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 1, sizeof( const cl_mem ), &livingCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // �� ������� �� ������������ � ���������� ��������� ���-�� ������.
    // �������������� ����������� ��������
    pd::zoneLivingCountComplete_t zoneCountComplete;
    static const size_t memsizeZoneCountComplete =
        sizeof( pd::zoneOneLivingCountComplete_t ) *
        pd::LIVING_COUNT * pd::LIFE_CYCLE_COUNT;
    const cl_mem zoneCountCompleteCL = clCreateBuffer(
        gpuContextCL,
        // ���. ������ �� ����������
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        memsizeZoneCountComplete,
        &zoneCountComplete,
        &errorCL
    );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 2, sizeof( const cl_mem ), &zoneCountCompleteCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // # �������� ��� ����� ������������� ��������� �������.
    const pd::LIFE_CYCLE lifeCycle = pd::LC_ADULT;
    errorCL = clSetKernelArg( kernel, 3, sizeof( pd::LIFE_CYCLE ), &lifeCycle );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 4, sizeof( const cl_mem ), &componentCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    errorCL = clSetKernelArg( kernel, 5, sizeof( const cl_mem ), &temperatureCL );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

    // ��� ������ ��������� ������ �������� ��� ������ ������ ����, �����
    // ������������� ���: � ���� OpenCL ������������� "���������� �����".
    const cl_uint rseed = randomGenerator();
    errorCL = clSetKernelArg( kernel, 6, sizeof( cl_uint ), &rseed );
    oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );


    const auto& al = tp.aboutPlanet.living;

    // �������� ���-�� ������
    float allZoneDesireCount = 0.0f;
    pd::zoneLivingCount_t zoneDesireCount = {};
    /* - ����� ���������� ���. ��. ����.
    const auto fnDesireCount = [ &zoneDesireCount, lifeCycle ] (
        float& zolValue,
        const pd::zoneLiving_t& zl
    ) -> void {
        const pd::CODE_LIVING code = zl.code;
        if (code != pd::CL_NONE) {
            const float q = zl.count;
            zolValue += q;
            zoneDesireCount[ code ][ lifeCycle ].all += q;
        }
    };
    */
    for (size_t k = 0; k < pd::LIVING_CELL; ++k) {
        // space
        {
            const auto& zk = al.space[k];
            const pd::CODE_LIVING code = zk.code;
            if (code != pd::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].space += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // atmosphere
        {
            const auto& zk = al.atmosphere[k];
            const pd::CODE_LIVING code = zk.code;
            if (code != pd::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].atmosphere += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // crust
        {
            const auto& zk = al.crust[k];
            const pd::CODE_LIVING code = zk.code;
            if (code != pd::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].crust += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // mantle
        {
            const auto& zk = al.mantle[k];
            const pd::CODE_LIVING code = zk.code;
            if (code != pd::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].mantle += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
        // core
        {
            const auto& zk = al.core[k];
            const pd::CODE_LIVING code = zk.code;
            if (code != pd::CL_NONE) {
                zoneDesireCount[ code ][ lifeCycle ].core += zk.count;
                zoneDesireCount[ code ][ lifeCycle ].all += zk.count;
                allZoneDesireCount += zk.count;
            }
        }
    } // for (size_t k


    // �������� init(), ����������� ���������� ���-��
    // @see ��������� ��� init().
    size_t iteration = 0;
    for ( ; ; ) {
        // �������������� ������
        // (��. ���� ����. ����� ������ ���� clear())
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );

        // ���������
        // ������ ����� - ��������������������� ���� ������ ����������,
        // ����������� - ��������� ������ ���� init()
        // @todo optimize ����������� ������������ livingCL ���
        //       ������� ������ � tp.living.content.
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

        // ������������ ���-��
        float allZoneCount = 0.0f;
        pd::zoneLivingCount_t zoneCount = {};
        // �������� �� ���� �������
        static const size_t LG = pd::LIVING_GRID * pd::LIVING_GRID * pd::LIVING_GRID;
        for (size_t i = 0; i < LG; ++i) {
            // � ������ ������ ���������� �� ����� LIVING_CELL ������
            for (size_t k = 0; k < pd::LIVING_CELL; ++k) {
                // ���������� �� ������ ������������� �� LIFE_CYCLE
                for (size_t l = static_cast< size_t >( pd::LC_EMBRYO);
                     l < static_cast< size_t >( pd::LC_last );  ++l
                ) {
                    const pd::portionLiving_t& p = tp.living.content[i][k][l];
                    const pd::CODE_LIVING code = p.code;
                    assert( ((code >= pd::CL_NONE) && (code < pd::CL_last))
                        && "��� ����� �� ���������. ��������, ������ ��� ������������� �������." );
                    // ��������� (������������� ���� ����)
                    zoneCount[ code ][ l ].all += p.count;
                    allZoneCount += p.count;
                }
            }
        }

        // ���-�� ������ ������ ���� *������* � ���������
        std::memset( zoneCountComplete, false, memsizeZoneCountComplete );
        bool allComplete = true;
        for (size_t code = 0; code < pd::LIVING_COUNT; ++code) {
            for (size_t l = static_cast< size_t >( pd::LC_EMBRYO);
                 l < static_cast< size_t >( pd::LC_last );  ++l
            ) {
                const float desireCount = zoneDesireCount[ code ][ l ].all;
                const float count       = zoneCount[ code ][ l ].all;
                // # �� ���������� ����������� �� �����.
                auto& zcc = zoneCountComplete[ code ][ l ];
                zcc.all = (desireCount <= count * 1.1f);
                if ( !zcc.all ) {
                    allComplete = false;
                    break;
                }
            }
            if ( !allComplete ) {
                // true ��� �� �����
                break;
            }

        } // for (size_t code

        if ( allComplete ) {
            // ������� ��������, ���������� ������ *����������* � ���������
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

        // �������������
        /* - (�������������� � ������ �����)
        errorCL = clFinish( commandQueueCL );
        oclCheckErrorEX( errorCL, CL_SUCCESS, &fnErrorCL );
        */

        // ���������
        // (�������� � ������ �����)

        ++iteration;

#ifdef _DEBUG
        //std::cout << ".";
        // ���������� ��������� ���������
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
    std::cout << " ��          " << std::endl;
#endif
}
#endif


} // porte
