#pragma once

#include <string>


namespace porte {


static const std::string PATH_CL =
    "D:/Projects/workspace/porte/porte/cl-kernel";
static const std::string PATH_CL_DUNGEONCRAWL =
    PATH_CL + "/set/dungeon-crawl";
static const std::string PATH_STRUCTURE_CL_DUNGEONCRAWL =
    "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl";



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
