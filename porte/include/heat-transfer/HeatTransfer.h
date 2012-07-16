#pragma once

#include "../porte/Engine.h"
#include <numeric>
#include <boost/assign.hpp>

/* - ���������� C-������ �� NVIDIA.
     ��. http://nvidia.com/content/cuda/cuda-downloads.html
#include "../../external/opencl/cl.hpp"
*/
#include <oclUtils.h>
#include <CL/cl_gl.h>    

// (i) ��� Apple: "cl_APPLE_gl_sharing"
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"



namespace porte {

template< size_t SX, size_t SY, size_t SZ >
class HeatTransfer;

}




namespace porte {


/**
* ������ �����������.
* ����� ���������� �������� ������ ����� � ����������� �� ���������� � �������
* � �������� �������.
*
* @template ��. Engine.
*/
template< size_t SX, size_t SY, size_t SZ >
class HeatTransfer : public Engine< SX, SY, SZ > {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< HeatTransfer >  Ptr;
    typedef std::unique_ptr< HeatTransfer >  UPtr;



public:
    HeatTransfer( portulan_t* );

    virtual ~HeatTransfer();




protected:
    /**
    * @virtual Engine
    */
    virtual void operator()();




private:
    void prepareCLKernel();
    static void fnErrorCL( int exitCode );




private:
    /**
    * ����������, �������� � ���� OpenCL ��� ������ � ���������� ����.
    */
    std::map< std::string, cl_kernel >  kernelCL;

    /* - �������� ������ prepareCLKernel().
    cl_int errorCL;
    cl_uint numDevicesCL;
    cl_device_id* devicesCL;
    cl_device_id deviceCL;
    cl_uint deviceUsedCL;
    cl_uint devCountCL;
    cl_platform_id platformCL;
    cl_program programCL;
    char* pathAndNameCL;
    char* sourceCL;
    */

    cl_context gpuContextCL;

    // @todo optimize? ������������ ����. �������� ������?
    cl_command_queue commandQueueCL;


    /**
    * ���������� ����� ������� � ����������� (�� ��� GPU ������������).
    * @see http://sa09.idav.ucdavis.edu/docs/SA09_GL_interop.pdf
    *//* - �� ���������� OpenGL, ������ ����������.
    cl_bool glInteropCL;
    */


    /**
    * ��������� ��� OpenCL-����������.
    * ������� "work" ����������� � ����������, ������� ������������ ������ ���
    * ������������� ����������.
    */
    cl_mem temperatureCL;
    cl_mem workTemperatureCL;

};




} // porte








#include "HeatTransfer.inl"
