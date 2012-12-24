#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "ListenerPlanet.h"

#include <numeric>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <ctime>


/* - ���. ���������� C-������ �� NVIDIA.
     ��. http://nvidia.com/content/cuda/cuda-downloads.html
#include "../../external/opencl/cl.hpp"
*/
#include <oclUtils.h>
#include <CL/cl_gl.h>    

// (i) ��� Apple: "cl_APPLE_gl_sharing"
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"


// ���������� ��������� define
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
* ������ ��� ������������� �������, ��������� ���������� �� Engine.
*
* @template ��. Engine.
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
    * ������.
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
    * �������������� ������� �������.
    */
    void init();


    /**
    * �������������� ���������� � ������� �������.
    */
    void initComponent();


    /**
    * �������������� ����� ���������� � ������� �������.
    */
    void initTemperature();


    /**
    * �������������� ����� ���������� �� ����������� �������.
    */
    void initSurfaceTemperature();


    /**
    * �������������� ����� ����������� ������� �� ����������� �������.
    */
    void initRainfall();


    /**
    * �������������� ����� ������� �� ����������� �������.
    */
    void initDrainage();


    /**
    * �������������� ����� ��������� ��������� �� ����������� �������.
    */
    void initLandscape();


    /**
    * �������������� ����� ������������ � ������� �������.
    */
    void initIlluminance();


    /**
    * �������������� ����� ������ � ������� �������.
    */
    void initBiome();


    /**
    * �������������� ����� ��������� � ������� �������.
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

    // # ������� ���������� ������� �����. ������� �������������.
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
    * ����������� ���� � ������ "G".
    *
    * @param kernelKeys  ���� ��� ����������.
    * @param includeHCL  ������������ � ����� ��������. ����������.
    * @param options     ��������. ��������� ����.
    */
    template< size_t G >
    void compileCLKernel(
        const std::vector< std::string >&  kernelKeys,
        const std::vector< std::string >&  includeHCL = std::vector< std::string >(),
        const std::string& options = ""
    );


    /**
    * ����� ��������� � ����� ��� �������� ���� OpenCL.
    *
    *   #! ��� ���������, ������������ � OpenCL ���� "define" ������� � ���� OpenCL
    *      ��������� � ��������� ���� ����. ������ ����� ��� float-��������, �������
    *      ����� ��������� ����� ������ �����. � ��������� ������, ���������� � ��
    *      �������� ����� ������������ ������������ ��� �������������. ������� - ������.
    */
    static std::string commonConstantCLKernel();
    static std::string commonOptionCLKernel();


    static void fnErrorCL( int exitCode );




private:
    /**
    * ����������, �������� � ���� OpenCL ��� ������ � ���������� ����.
    */
    std::map< std::string, cl_kernel >  kernelCL;

    /**
    * ��������� ��� OpenCL-����������.
    *   # ������� "work" ����������� � ���������, ������� ������������ ������ ���
    *     ������������� ����������.
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
    * ��������� ����. �����.
    */
    boost::mt19937 randomCore;
    boost::uniform_int< cl_uint >  randomDistribution;
    boost::variate_generator<
        boost::mt19937&, boost::uniform_int< unsigned int >
    > randomGenerator;
    cl_uint randomSeed;


    /**
    * ��������������� ����������.
    */
    cl_int errorCL;

    cl_device_id* devicesCL;
    cl_uint deviceUsedCL;
    cl_uint devCountCL;
    cl_platform_id platformCL;

    cl_context gpuContextCL;

    // @todo optimize? ������������ ����. �������� ������?
    cl_command_queue commandQueueCL;
};





void __stdcall pfn_notify_cl(
    const char* errinfo, const void* private_info,
    size_t cb, void* user_data
);


/* - �� ������, ������ ����� ����� ����������.
void __stdcall pfn_notify_program_cl(
    cl_program, void* user_data
);
*/


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte







// ������� �� ����. ������ ��� ��������
#include "Engine.inl"
#include "prepare.inl"
#include "init.inl"
