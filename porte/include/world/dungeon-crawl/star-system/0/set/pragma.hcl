// Потребность в директивах для разных версий OpenCL...
// #! При работе через OpenCL 1.1 на CPU - ошибка на прагмах ниже.
#if __OPENCL_VERSION__ == 100

/* @todo В звёздной системе работать с двойной точностью.
//       NVIDIA 8800GTS работает только с float.
#ifdef cl_khr_fp64
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

#ifdef cl_amd_fp64
    #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif
*/


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
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

#endif
