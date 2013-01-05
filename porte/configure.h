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
* Элементы мира Dungeon Crawl, которые будут включены в движок.
* Отключается, как правило, для отладки.
*   # Необходимо помнить о зависимостях элементов друг от друга.
*/
#if 1
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
* Настраивает движок планетной системы для работы на GPU OpenCL.
* Если не декларирован, движок работает на CPU OpenCL.
*/
//#define GPU_OPENCL_PLANET_PORTE



/**
* Включает движок звёздной системы на работу через OpenCL. Иначе,
* подключается движок расчётов на CPU.
*
* #! OpenCL не работает с double на NVIDIA GeForce 8800GTS.
*//* - Подключается в main-проектах. См. 'process-visualtest'.
//#define OPENCL_STARSYSTEM_L0_ENGINE_PORTE
#define ND_STARSYSTEM_L0_ENGINE_PORTE
*/


} // porte
