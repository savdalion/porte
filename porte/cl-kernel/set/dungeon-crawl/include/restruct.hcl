/**
* (!) Все определения из этого файла должны быть согласованы со
* структурами C++.
*
*   # Структуры C++ в portulan построены так, чтобы не дублировать код.
*/

#define PORTULAN_AS_OPEN_CL_STRUCT

// Декларируем типы OpenCL чтобы подключаемые структуры на C++
// не уродовать if-def'ами.
typedef float   cl_float;
typedef int     cl_int;
typedef uint    cl_uint;
typedef ulong   cl_ulong;
typedef ushort  cl_ushort;

#define CL_FLT_MAX          340282346638528859811704183484516925440.0f


// #! Важен порядок подключения.
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/structure.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/component.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/living.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/temperature.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/planet.h"
//#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/topology.h"
