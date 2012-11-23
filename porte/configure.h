#pragma once

#include <string>


namespace porte {


static const std::string PATH_CL_PORTE =
    "D:/Projects/workspace/porte/porte/cl-kernel";
static const std::string L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTE =
    PATH_CL_PORTE + "/world/dungeon-crawl/planet/0";

static const std::string PATH_CL_PORTULAN =
    "D:/Projects/workspace/portulan/portulan/include";
static const std::string L0_PLANET_DUNGEONCRAWL_PATH_CL_PORTULAN =
    PATH_CL_PORTULAN + "/world/dangeon-crawl/planet/0";



/**
* Элементы мира Dungeon Crawl, которые будут включены в движок.
* Отключается, как правило, для отладки.
*   # Необходимо помнить о зависимостях элементов друг от друга.
*/
#define COMPONENT_DUNGEONCRAWL_PORTE
#define TEMPERATURE_DUNGEONCRAWL_PORTE
#define SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
#define RAINFALL_DUNGEONCRAWL_PORTE
#define DRAINAGE_DUNGEONCRAWL_PORTE
#define LANDSCAPE_DUNGEONCRAWL_PORTE
#define BIOME_DUNGEONCRAWL_PORTE
#define LIVING_DUNGEONCRAWL_PORTE


} // porte
