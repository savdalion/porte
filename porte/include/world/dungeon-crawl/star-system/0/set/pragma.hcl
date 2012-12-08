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

#endif
