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


const std::string PATH_CL = "D:/Projects/workspace/porte/porte/cl-kernel";
const std::string PATH_CL_DUNGEONCRAWL = PATH_CL + "/set/dungeon-crawl";

} // porte
