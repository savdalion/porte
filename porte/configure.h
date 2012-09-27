#pragma once

#include <string>


namespace porte {


/**
* При использовании этого ключа, ядра OpenCL будут компилироваться при каждом
* запуске приложения. Это может существенно замедлить запуск программы, но
* гарантирует, что все изменения кода ядра будут учтены.
*
* Причина ввода этого ключа.
* Если ядро включает множество файлов (#include), OpenCL (по крайней мере от
* NVidia) не учитывает изменения в них. В результате получаются феноменальные
* ошибки аля "код работает через раз".
*/
//#define ALWAYS_BUILD_CL_KERNEL_PORTE


static const std::string PATH_CL =
    "D:/Projects/workspace/porte/porte/cl-kernel";
static const std::string PATH_CL_DUNGEONCRAWL =
    PATH_CL + "/set/dungeon-crawl";



/**
* Элементы мира Dungeon Crawl, которые будут включены в движок.
* Отключается, как правило, для отладки.
*   # Необходимо помнить о зависимостях элементов друг от друга.
*/
#define COMPONENT_DUNGEONCRAWL_PORTE
#define LIVING_DUNGEONCRAWL_PORTE
#define TEMPERATURE_DUNGEONCRAWL_PORTE


} // porte
