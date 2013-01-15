#pragma once

#include <string>


namespace porte {


static const std::string PATH_CL_PORTE =
    "D:/Projects/workspace/porte/porte/include";
static const std::string L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE =
    PATH_CL_PORTE + "/world/dungeon-crawl/planet/0";
static const std::string L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTE =
    PATH_CL_PORTE + "/world/dungeon-crawl/star-system/0";

static const std::string PATH_CL_PORTULAN =
    "D:/Projects/workspace/portulan/portulan/include";
static const std::string L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN =
    PATH_CL_PORTULAN + "/world/dangeon-crawl/planet/0";
static const std::string L0_STARSYSTEM_DUNGEONCRAWL_PATH_CL_PORTULAN =
    PATH_CL_PORTULAN + "/world/dangeon-crawl/star-system/0";



/**
* �������� ���� Dungeon Crawl, ������� ����� �������� � ������.
* �����������, ��� �������, ��� �������.
*   # ���������� ������� � ������������ ��������� ���� �� �����.
*/
#if 0
#define COMPONENT_DUNGEONCRAWL_PORTE
#define TEMPERATURE_DUNGEONCRAWL_PORTE
#define SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
#define RAINFALL_DUNGEONCRAWL_PORTE
#define DRAINAGE_DUNGEONCRAWL_PORTE
#define LANDSCAPE_DUNGEONCRAWL_PORTE
#define ILLUMINANCE_DUNGEONCRAWL_PORTE
#define BIOME_DUNGEONCRAWL_PORTE
#define LIVING_DUNGEONCRAWL_PORTE
#endif




/**
* ����������� ������ ��������� ������� ��� ������ �� GPU OpenCL.
* ���� �� ������������, ������ �������� �� CPU OpenCL.
*/
//#define GPU_OPENCL_PLANET_PORTE



/**
* �������� ������ ������� ������� �� ������ ����� OpenCL. �����,
* ������������ ������ �������� �� CPU.
*
* #! OpenCL �� �������� � double �� NVIDIA GeForce 8800GTS.
*//* - ������������ � main-��������. ��. 'process-visualtest'.
//#define OPENCL_STARSYSTEM_L0_ENGINE_PORTE
#define ND_STARSYSTEM_L0_ENGINE_PORTE
*/




/**
* ��������� ������������ ��� 'double' � ������� PORTE.
* ��� ���� ��������� ������ ��������� ����������� � 'float'.
* 
* #! �� ��� ���������� ������������ 'double' ��� OpenCL.
*
* @see typedef real_t
* @see portulan::PERMIT_DOUBLE_ENGINE_PORTULAN
*/
#define PERMIT_DOUBLE_ENGINE_PORTE




/**
* ��������� ��������, ��� ������� ����� �������������� �����. ������� ������.
*/
static const auto MIN_IMPACT_FORCE_PORTE = 1.0f;
static const auto MIN_CHANGE_VELOCITY_PORTE = 0.1f;
static const auto MIN_CHANGE_DISTANCE_PORTE = 0.01f;



} // porte
