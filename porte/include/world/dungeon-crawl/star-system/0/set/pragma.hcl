// ����������� � ���������� ��� ������ ������ OpenCL...
// #! ��� ������ ����� OpenCL 1.1 �� CPU - ������ �� ������� ����.
#if __OPENCL_VERSION__ == 100

/* @todo � ������� ������� �������� � ������� ���������.
//       NVIDIA 8800GTS �������� ������ � float.
#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

#ifdef cl_amd_fp64
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
*/


// ��������� ������ � �������� ��������.
// @todo optimize? �������� ������ � �������� ����������?
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

// �������� ������� ����������������� ��������� � ������.
// @see OpenCL / atomic
// @todo optimize? ��������� ��������� ������ � �������?
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable


#else

/* - @todo ? �� �������� �� CPU Intel i5.
#pragma OPENCL EXTENSION cl_intel_printf : enable
#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#else
    #error "Double precision floating point not supported by OpenCL implementation."
#endif
*/

#pragma EXTENSION cl_khr_fp64 : require
#pragma EXTENSION cl_intel_printf : require
// @todo ? ���� ��������� ���� ���������� �� �������� ������� �������� ��� CPU Intel i5.
#pragma OPENCL EXTENSION all : enable


#endif
