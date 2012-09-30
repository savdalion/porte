#pragma once

#include "../porte/Engine.h"
#include <numeric>
#include <boost/assign.hpp>
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

template< size_t SX, size_t SY, size_t SZ >
class Chemistry;

}




namespace porte {


/**
* ������ ���������� �����������.
* ����������� ����������� ������ �����.
*
* @template ��. Engine.
*/
template< size_t SX, size_t SY, size_t SZ >
class Chemistry : public Engine< SX, SY, SZ > {
public:
    /**
    * ������.
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
    * ����������, �������� � ���� OpenCL ��� ������ � ���������� ����.
    */
    std::map< std::string, cl_kernel >  kernelCL;

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
    * ������� "work" ����������� � ���������, ������� ������������ ������ ���
    * ������������� ����������.
    */
    cl_mem boosterCL;
    cl_mem workBoosterCL;

};




} // porte








#include "Chemistry.inl"
