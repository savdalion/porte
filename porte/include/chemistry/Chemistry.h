#pragma once

#include "../porte/Engine.h"
#include <numeric>
#include <boost/assign.hpp>
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

template< size_t SX, size_t SY, size_t SZ >
class Chemistry;

}




namespace porte {


/**
* Движок химических превращений.
* Превращения совершаются каждый пульс.
*
* @template См. Engine.
*/
template< size_t SX, size_t SY, size_t SZ >
class Chemistry : public Engine< SX, SY, SZ > {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< Chemistry >  Ptr;
    typedef std::unique_ptr< Chemistry >  UPtr;



public:
    Chemistry( portulan_t* );

    virtual ~Chemistry();




protected:
    /**
    * @virtual Engine
    */
    virtual void pulse( int n );




private:
    void prepare();
    static void fnErrorCL( int exitCode );




private:
    /**
    * Устройства, контекст и ядра OpenCL для работы с элементами мира.
    */
    std::map< std::string, cl_kernel >  kernelCL;

    cl_context gpuContextCL;

    // @todo optimize? Использовать неск. очередей команд?
    cl_command_queue commandQueueCL;


    /**
    * Ускоренный обмен данными с видеокартой (не все GPU поддерживают).
    * @see http://sa09.idav.ucdavis.edu/docs/SA09_GL_interop.pdf
    *//* - Не используем OpenGL, только вычисления.
    cl_bool glInteropCL;
    */


    /**
    * Структуры для OpenCL-вычислений.
    * Префикс "work" добавляется к структуре, которая используются ядрами для
    * промежуточных вычислений.
    */
    cl_mem boosterCL;
    cl_mem workBoosterCL;

};




} // porte








#include "Chemistry.inl"
