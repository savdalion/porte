/**
* (!) Все определения из этого файла должны быть согласованы со
* структурами C++.
*
*   # Структуры C++ в portulan построены так, чтобы не дублировать код.
*/

// Декларируем типы OpenCL, чтобы подключаемые структуры на C++
// не уродовать if-def'ами.

typedef double   real_t;
typedef double2  real2_t;
typedef double3  real3_t;
typedef double4  real4_t;

typedef char     cl_char;
typedef uchar    cl_uchar;
typedef int      cl_int;
typedef uint     cl_uint;
typedef long     cl_long;
typedef ulong    cl_ulong;
typedef short    cl_short;
typedef ushort   cl_ushort;
typedef real_t   cl_real;
typedef real2_t  cl_real2;
typedef real3_t  cl_real3;
typedef real4_t  cl_real4;

#define nullptr  0


// #! Важен порядок подключения. Иначе при компиляции ядра рискуем получить
//    феноменальные ошибки... в которых нет мало-мальского сообщения о
//    причине ошибке.
// #! Символ Unicode в ядре OpenCL (появиться может, например, после копирования
//    справки из Википедии) или файл, сохранённый в Unicode, может привести
//    к феноменальным ошибкам.
// # Хорошее решение для отладки феноменальных ошибок > https://sourceforge.net/projects/clcc

// @see Соглашения в helper.hcl.
