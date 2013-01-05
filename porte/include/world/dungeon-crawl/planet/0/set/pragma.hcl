// #! CPU не понимает '#pragma OPENCL'.
#ifdef GPU_OPENCL

// Разрешаем запись в короткие регистры.
// @todo optimize? Работать только с длинными регистрами?
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

// Включаем функции последовательного обращения к памяти.
// @see OpenCL / atomic
// @todo optimize? Разрешить свободную работу с памятью?
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

#endif
