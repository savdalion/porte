#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "ListenerStarSystem.h"
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
namespace pep = porte::world::dungeoncrawl::planet::l0;


/**
* ������ ��� ������������� ������� �������.
*/
class EngineHybrid :
    public EngineWithoutBooster< pns::Portulan, pns::real_t >
{
public:
    typedef std::shared_ptr< EngineHybrid >  Ptr;
    typedef std::unique_ptr< EngineHybrid >  UPtr;



public:
    EngineHybrid( real_t timestep );


    virtual ~EngineHybrid();




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );
    void pulse();




private:
    bool emitEvent();




    /**
    * ����� ���� ��������� �� ���� A �� ������� B -> � 'force'.
    *
    * @return false ���� ���� ����������� (������������ ����������
    *         'noForceDistance' - ���� ���������� �� �����������).
    *         true  ���� ���� A � B ��������� �� ���������, �����������
    *         'noForceDistance'.
    *
    * @param noForceDistance ����������, �� ������� ���� ����� ���������. �.�.
    *        �������� ������� ������ ���������.
    */
    static bool forceGravityBodyAImpactIn(
        real_t force[ 3 ],
        const pns::aboutBody_t& a,
        const pns::aboutBody_t& b,
        real_t noForceDistance
    );




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
#include "prepareOpenCL.inl"
