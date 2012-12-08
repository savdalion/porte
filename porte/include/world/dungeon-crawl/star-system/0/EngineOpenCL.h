#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include <numeric>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <ctime>

// # Все комментарии см. в planet::l0::Engine.

#include <oclUtils.h>
#include <CL/cl_gl.h>    

#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"


#undef CL_NONE


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pd = portulan::world::dungeoncrawl::starsystem::l0;


/**
* Движок для моделирования звёздной системы.
*/
class EngineOpenCL :
    public EngineWithoutBooster< pd::Portulan >
{
public:
    typedef std::shared_ptr< EngineOpenCL >  Ptr;
    typedef std::unique_ptr< EngineOpenCL >  UPtr;



public:
    EngineOpenCL( portulan_t*, pd::real_t timestep );

    virtual ~EngineOpenCL();




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );




private:
    void prepareCLContext();
    void prepareCLCommandQueue();

    void prepare();
    void prepareGravity();


    void compileCLKernel(
        const std::vector< std::string >&  kernelKeys,
        const std::vector< std::string >&  includeHCL = std::vector< std::string >(),
        const std::string& options = ""
    );


    static std::string commonConstantCLKernel();
    static std::string commonOptionCLKernel();


    static void fnErrorCL( int exitCode );




private:
    std::map< std::string, cl_kernel >  kernelCL;

    pd::real_t mTimestep;

    const size_t memsizeBody;
    const size_t memsizeStarSystem;


    cl_int errorCL;

    cl_device_id* devicesCL;
    cl_uint deviceUsedCL;
    cl_uint devCountCL;
    cl_platform_id platformCL;

    cl_context gpuContextCL;

    cl_command_queue commandQueueCL;
};






void __stdcall pfn_notify_cl(
    const char* errinfo, const void* private_info,
    size_t cb, void* user_data
);




/**
* Получает платформу OpenCL, отдавая предпочтение CPU.
*
* @see oclUtils.h / oclGetPlatformID()
*/
cl_int getPlatformIDCPU( cl_platform_id* clSelectedPlatformID );



                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineOpenCL.inl"
#include "prepareOpenCL.inl"
