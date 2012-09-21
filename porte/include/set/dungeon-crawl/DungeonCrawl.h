#pragma once

#include "../../porte/EngineWithoutBooster.h"
#include <numeric>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <ctime>


/* - ���. ���������� C-������ �� NVIDIA.
     ��. http://nvidia.com/content/cuda/cuda-downloads.html
#include "../../external/opencl/cl.hpp"
*/
#include <oclUtils.h>
#include <CL/cl_gl.h>    

// (i) ��� Apple: "cl_APPLE_gl_sharing"
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"




namespace porte {

class DungeonCrawl;

}




namespace porte {


/**
* ������ ��� ������������� �������, ��������� ���������� �� DungeonCrawl.
*
* @template ��. Engine.
*/
class DungeonCrawl : public EngineWithoutBooster {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< DungeonCrawl >  Ptr;
    typedef std::unique_ptr< DungeonCrawl >  UPtr;



public:
    DungeonCrawl( portulan_t* );

    virtual ~DungeonCrawl();



    /**
    * �������������� ������� �������.
    */
    void init();


    /**
    * �������������� ���������� � ������� �������.
    */
    void initComponent();


    /**
    * �������������� ����� ��������� � ������� �������.
    */
    void initLiving();

    
    /**
    * �������������� ����� ���������� � ������� �������.
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




} // porte








#include "DungeonCrawl.inl"
