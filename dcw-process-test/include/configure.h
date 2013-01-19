#pragma once

#include <string>


namespace porte {
    namespace dcwtest {


/**
* Варианты движков для моделирования звёздной системы.
*
* # Допустимо выбрать только один движок.
* # Если не указан, выбирается гибридный.
*
* #! OpenCL не работает с double на NVIDIA GeForce 8800GTS.
*/
// Движок на параллельном OpenCL.
//#define OPENCL_STARSYSTEM_L0_ENGINE_PORTE
// Для движка на OpenCL: указание использовать CPU.
#define ONLY_CPU_OPENCL_STARSYSTEM_L0_ENGINE_PORTE

// Движок на линейном CPU.
//#define CPU_STARSYSTEM_L0_ENGINE_PORTE

// Движок на NewtonDynamics.
// #! Выяснено: Использовать для систем с огромными расстояниями...
//    не практично.
//#define ND_STARSYSTEM_L0_ENGINE_PORTE


    } // dcwtest
} // porte
