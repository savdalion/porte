#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "ListenerPlanet.h"

#include <numeric>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <ctime>


/* - Нет. Используем C-подход от NVIDIA.
     См. http://nvidia.com/content/cuda/cuda-downloads.html
#include "../../external/opencl/cl.hpp"
*/
#include <oclUtils.h>
#include <CL/cl_gl.h>    

// (i) Для Apple: "cl_APPLE_gl_sharing"
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"


// существует неудобный define
#undef CL_NONE



namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;


/**
* Движок для моделирования планеты, населённой созданиями из Engine.
*
* @template См. Engine.
*/
class Engine :
    public EngineWithoutBooster<
        pnp::Portulan,
        float
    >,
    public ListenerPlanet< Engine >
{
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< Engine >  Ptr;
    typedef std::unique_ptr< Engine >  UPtr;



public:
    Engine( real_t timestep );

    virtual ~Engine();




    /**
    * @virtual 
    */
    virtual void incarnate( std::unique_ptr< portulan_t >,  real_t extentPortulan = 0 );




    /**
    * @virtual 
    */
    virtual real_t extent();




    /**
    * Инициализирует область планеты.
    */
    void init();


    /**
    * Инициализирует компоненты в области планеты.
    */
    void initComponent();


    /**
    * Инициализирует сетку температур в области планеты.
    */
    void initTemperature();


    /**
    * Инициализирует сетку температур на поверхности планеты.
    */
    void initSurfaceTemperature();


    /**
    * Инициализирует сетку атмосферных осадков на поверхности планеты.
    */
    void initRainfall();


    /**
    * Инициализирует сетку дренажа на поверхности планеты.
    */
    void initDrainage();


    /**
    * Инициализирует сетку элементов ландшафта на поверхности планеты.
    */
    void initLandscape();


    /**
    * Инициализирует сетку освещённости в области планеты.
    */
    void initIlluminance();


    /**
    * Инициализирует сетку биомов в области планеты.
    */
    void initBiome();


    /**
    * Инициализирует живые организмы в области планеты.
    */
    void initLiving();


    

protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );
    void pulse();




private:
    void prepareCLContext();
    void prepareCLCommandQueue();

    // # Порядок следования методов соотв. порядку инициализации.
    void prepare();
    void prepareComponent();
    void prepareTemperature();
    void prepareSurfaceTemperature();
    void prepareRainfall();
    void prepareDrainage();
    void prepareLandscape();
    void prepareIlluminance();
    void prepareBiome();
    void prepareLiving();


    /**
    * Компилирует ядра с сеткой "G".
    *
    * @param kernelKeys  Ядра для компиляции.
    * @param includeHCL  Используемые в ядрах дополнит. библиотеки.
    * @param options     Дополнит. параметры ядра.
    */
    template< size_t G >
    void compileCLKernel(
        const std::vector< std::string >&  kernelKeys,
        const std::vector< std::string >&  includeHCL = std::vector< std::string >(),
        const std::string& options = ""
    );


    /**
    * Общие константы и опции для передачи ядру OpenCL.
    *
    *   #! Все константы, отправляемые в OpenCL путём "define" следует в коде OpenCL
    *      приводить к желаемому типу явно. Крайне важно для float-констант, которые
    *      вдруг оказались равны целому числу. В противном случае, вычисления с их
    *      участием могут восприняться компилятором как целочисленные. Словить - трудно.
    */
    static std::string commonConstantCLKernel();
    static std::string commonOptionCLKernel();


    static void fnErrorCL( int exitCode );




private:
    /**
    * Устройства, контекст и ядра OpenCL для работы с элементами мира.
    */
    std::map< std::string, cl_kernel >  kernelCL;

    /**
    * Структуры для OpenCL-вычислений.
    *   # Префикс "work" добавляется к структуре, которая используются ядрами для
    *     промежуточных вычислений.
    *
    * @see Engine::incarnate()
    */
    cl_mem aboutPlanetCL;
    const size_t memsizeAboutPlanet;

    const size_t memsizeComponent;
    const size_t memsizeTemperature;
    const size_t memsizeSurfaceTemperature;
    const size_t memsizeRainfall;
    const size_t memsizeDrainage;
    const size_t memsizeLandscape;
    const size_t memsizeIlluminance;
    const size_t memsizeBiome;
    const size_t memsizeLiving;


    /**
    * Генератор случ. чисел.
    */
    boost::mt19937 randomCore;
    boost::uniform_int< cl_uint >  randomDistribution;
    boost::variate_generator<
        boost::mt19937&, boost::uniform_int< unsigned int >
    > randomGenerator;
    cl_uint randomSeed;


    /**
    * Вспомогательные переменные.
    */
    cl_int errorCL;

    cl_device_id* devicesCL;
    cl_uint deviceUsedCL;
    cl_uint devCountCL;
    cl_platform_id platformCL;

    cl_context gpuContextCL;

    // @todo optimize? Использовать неск. очередей команд?
    cl_command_queue commandQueueCL;
};





void __stdcall pfn_notify_cl(
    const char* errinfo, const void* private_info,
    size_t cb, void* user_data
);


/* - Не ошибка, только вызов после компиляции.
void __stdcall pfn_notify_program_cl(
    cl_program, void* user_data
);
*/


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte







// разбито на неск. файлов для удобства
#include "Engine.inl"
#include "prepare.inl"
#include "init.inl"
