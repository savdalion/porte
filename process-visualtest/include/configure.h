#pragma once

#include <string>


namespace porte {
    namespace visualtest {


/**
* ����������� ����� ���� ������������ ��� ���������� ���������� ������
* � ������.
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
* �������� ������� ��� ������������� ������� �������.
*
* # ��������� ������� ������ ���� ������.
*
* #! OpenCL �� �������� � double �� NVIDIA GeForce 8800GTS.
*/
// ������ �� ������������ OpenCL.
//#define OPENCL_STARSYSTEM_L0_ENGINE_PORTE
// ��� ������ �� OpenCL: �������� ������������ CPU.
//#define ONLY_CPU_OPENCL_STARSYSTEM_L0_ENGINE_PORTE

// ������ �� �������� CPU.
#define CPU_STARSYSTEM_L0_ENGINE_PORTE

// ������ �� NewtonDynamics.
// #! ��������: ������������ ��� ������ � ��������� ������������...
//    �� ���������.
//#define ND_STARSYSTEM_L0_ENGINE_PORTE


/**
* ��������� ������������ ��� 'double' � ������� PORTE.
* ��� ���� ��������� ������ ��������� ����������� � 'float'.
* 
* #! �� ��� ���������� ������������ 'double' ��� OpenCL.
*
* @see typedef real_t
*/
#define PERMIT_DOUBLE_ENGINE_PORTE


    } // visualtest
} // porte
