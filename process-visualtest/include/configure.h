#pragma once

#include <string>


namespace porte {
    namespace visualtest {


/**
* Определения могут быть использованы для отключения сохранения данных
* в файлах.
*/
/*
#define COMPONENT_SNAPSHOT_VISUALTEST
#define TEMPERATURE_SNAPSHOT_VISUALTEST
#define SURFACE_TEMPERATURE_SNAPSHOT_VISUALTEST
#define RAINFALL_SNAPSHOT_VISUALTEST
#define DRAINAGE_SNAPSHOT_VISUALTEST
#define LANDSCAPE_SNAPSHOT_VISUALTEST
#define BIOME_SNAPSHOT_VISUALTEST
#define LIVING_SNAPSHOT_VISUALTEST
*/


/**
* Варианты движков для моделирования звёздной системы.
*
* # Допустимо выбрать только один движок.
*
* #! OpenCL не работает с double на NVIDIA GeForce 8800GTS.
*/
// Движок на параллельном OpenCL.
//#define OPENCL_STARSYSTEM_L0_ENGINE_PORTE
// Для движка на OpenCL: указание использовать CPU.
//#define ONLY_CPU_OPENCL_STARSYSTEM_L0_ENGINE_PORTE

// Движок на линейном CPU.
#define CPU_STARSYSTEM_L0_ENGINE_PORTE

// Движок на NewtonDynamics.
// #! Выяснено: Использовать для систем с огромными расстояниями...
//    не практично.
//#define ND_STARSYSTEM_L0_ENGINE_PORTE


/**
* Разрешает использовать тип 'double' в движках PORTE.
* Без этой директивы движки стараются поместиться в 'float'.
* 
* #! Не все видеокарты поддерживают 'double' для OpenCL.
*
* @see typedef real_t
*/
#define PERMIT_DOUBLE_ENGINE_PORTE


    } // visualtest
} // porte
