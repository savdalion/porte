#pragma once

#include "../../porte/EngineWithoutBooster.h"
#include <numeric>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <ctime>


/* - Нет. Используем C-подход от NVIDIA.
     См. http://nvidia.com/content/cuda/cuda-downloads.html
#include "../../external/opencl/cl.hpp"
*/
#include <oclUtils.h>
#include <CL/cl_gl.h>    

// (i) Для Apple: "cl_APPLE_gl_sharing"
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"




namespace porte {

class DungeonCrawl;

}




namespace porte {


/**
* Движок для моделирования планеты, населённой созданиями из DungeonCrawl.
*
* @template См. Engine.
*/
class DungeonCrawl : public EngineWithoutBooster {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< DungeonCrawl >  Ptr;
    typedef std::unique_ptr< DungeonCrawl >  UPtr;



public:
    DungeonCrawl( portulan_t* );

    virtual ~DungeonCrawl();



    /**
    * Инициализирует область планеты.
    */
    void init();


    /**
    * Инициализирует компоненты в области планеты.
    */
    void initComponent();


    /**
    * Инициализирует живые организмы в области планеты.
    */
    void initLiving();

    
    /**
    * Инициализирует сетку температур в области планеты.
    */
    void initTemperature();




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );




private:
    void prepareCLContext();
    void prepareCLCommandQueue();

    void prepareCLKernel();
    void prepareComponentCLKernel();
    void prepareLivingCLKernel();
    void prepareTemperatureCLKernel();


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
    */
    cl_mem aboutPlanetCL;
    const size_t memsizeAboutPlanet;

    cl_mem componentCL;
    cl_mem workComponentCL;
    const size_t memsizeComponent;

    cl_mem livingCL;
    cl_mem workLivingCL;
    const size_t memsizeLiving;

    cl_mem temperatureCL;
    cl_mem workTemperatureCL;
    const size_t memsizeTemperature;


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




} // porte








#include "DungeonCrawl.inl"
