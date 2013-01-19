#pragma once

#include <string>


namespace porte {
    namespace dcwtest {


/**
* �������� ������� ��� ������������� ������� �������.
*
* # ��������� ������� ������ ���� ������.
* # ���� �� ������, ���������� ���������.
*
* #! OpenCL �� �������� � double �� NVIDIA GeForce 8800GTS.
*/
// ������ �� ������������ OpenCL.
//#define OPENCL_STARSYSTEM_L0_ENGINE_PORTE
// ��� ������ �� OpenCL: �������� ������������ CPU.
#define ONLY_CPU_OPENCL_STARSYSTEM_L0_ENGINE_PORTE

// ������ �� �������� CPU.
//#define CPU_STARSYSTEM_L0_ENGINE_PORTE

// ������ �� NewtonDynamics.
// #! ��������: ������������ ��� ������ � ��������� ������������...
//    �� ���������.
//#define ND_STARSYSTEM_L0_ENGINE_PORTE


    } // dcwtest
} // porte
