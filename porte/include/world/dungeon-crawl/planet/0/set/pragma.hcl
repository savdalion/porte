// #! CPU �� �������� '#pragma OPENCL'.
#ifdef GPU_OPENCL

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
//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

#endif
