// @require pragma.hcl


/**
* (!) Все определения из этого файла должны быть согласованы со
* структурами C++.
*
*   # Структуры C++ в portulan построены так, чтобы не дублировать код.
*/

#define PORTULAN_AS_OPEN_CL_STRUCT

// Декларируем типы OpenCL чтобы подключаемые структуры на C++
// не уродовать if-def'ами.

/* - @see pragma.hcl
// Увы, не все видеокарты поддерживают 'double' для OpenCL.
// #! Типы данных должны быть согласованы с OpenCL HOST.
// # double4 поддерживается не всеми устройствами OpenCL. Не используем.
#ifdef PERMIT_DOUBLE_ENGINE_PORTE
typedef double  real_t;
#else
typedef float   real_t;
#endif
*/
typedef float   real_t;
typedef float2  real2_t;
typedef float4  real4_t;

typedef int     cl_int;
typedef uint    cl_uint;
typedef ulong   cl_ulong;
typedef ushort  cl_ushort;
typedef float   cl_float;

#define nullptr  0


// #! Важен порядок подключения. Иначе при компиляции ядра рискуем получить
//    феноменальные ошибки... в которых нет мало-мальского сообщения о
//    причине ошибке.
// #! Символ Unicode в ядре OpenCL (появится может, например, после копирования
//    справки из Википедии) или файл, сохранённый в Unicode, может привести
//    к феноменальным ошибкам.
// # Хорошее решение для отладки феноменальных ошибок > https://sourceforge.net/projects/clcc

// @see Соглашения в helper.hcl.


// # Include-файлы подключаем в коде C++ при сборке ядра.
