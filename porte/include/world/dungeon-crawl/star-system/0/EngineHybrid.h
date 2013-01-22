#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "StoreListenerAsteroid.h"
#include "StoreListenerStar.h"
#include <boost/assign.hpp>

// # ��� ����������� ��. � planet::l0::Engine.

#include <oclUtils.h>
#include <CL/cl_gl.h>    

#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"


#undef CL_NONE


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


/**
* ������ ��� ������������� ������� �������.
*/
class EngineHybrid :
    public EngineWithoutBooster< pns::Portulan, pns::real_t >,
    // # �� ������� ����� ������ ����� �����������.
    public StoreListenerAsteroid,
    public StoreListenerStar
{
public:
    typedef std::shared_ptr< EngineHybrid >  Ptr;
    typedef std::unique_ptr< EngineHybrid >  UPtr;



public:
    EngineHybrid( real_t timestep );


    virtual ~EngineHybrid();




    /**
    * @virtual EngineWithoutBooster
    */
    virtual void incarnate(
        std::shared_ptr< portulan_t >  p,
        real_t extentPortulan = 0
    );




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );




private:
    void emitEvent( int n );




    /**
    * @return ���� ����������� (������������ ���������� 'distance').
    */
    static bool collision(
        const real_t coordA[ 3 ],
        const real_t coordB[ 3 ],
        real_t collisionDistance
    );




    /**
    * �������� ���������� ��� ��������� ���������.
    */
    void statistics();




    void prepareCLContext();
    void prepareCLCommandQueue();

    void prepare();
    void prepareEmitEvent();


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

    const size_t memsizeStarSystem;
    const size_t memsizeAsteroid;
    const size_t memsizePlanet;
    const size_t memsizeStar;

    cl_mem aboutStarSystemCL;
    cl_mem asteroidCL;
    cl_mem planetCL;
    cl_mem starCL;


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
* �������� ��������� OpenCL, ������� ������������ CPU.
*
* @see oclUtils.h / oclGetPlatformID()
*/
cl_int getPlatformIDCPU( cl_platform_id* clSelectedPlatformID );


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineHybrid.inl"
#include "prepareEngineHybrid.inl"
