// @require pragma.hcl


/**
* (!) ��� ����������� �� ����� ����� ������ ���� ����������� ��
* ����������� C++.
*
*   # ��������� C++ � portulan ��������� ���, ����� �� ����������� ���.
*/

#define PORTULAN_AS_OPEN_CL_STRUCT

// ����������� ���� OpenCL ����� ������������ ��������� �� C++
// �� ��������� if-def'���.
typedef float   cl_float;
typedef int     cl_int;
typedef uint    cl_uint;
typedef ulong   cl_ulong;
typedef ushort  cl_ushort;

#define CL_FLT_MAX          340282346638528859811704183484516925440.0f


// #! ����� ������� �����������. ����� ��� ���������� ���� ������� ��������
//    ������������� ������... � ������� ��� ����-��������� ��������� �
//    ������� ������.
// #! ������ Unicode � ���� OpenCL (�������� �����, ��������, ����� �����������
//    ������� �� ���������) ��� ����, ���������� � Unicode, ����� ��������
//    � ������������� �������.
// # ������� ������� ��� ������� ������������� ������ > https://sourceforge.net/projects/clcc

// @see ���������� � helper.hcl.


/* - ���������� � ���� C++ ��� ������ ����.
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/structure.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/component.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/living.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/temperature.h"
#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/planet.h"
//#include "D:/Projects/workspace/portulan/portulan/include/planet/set/dangeon-crawl/topology.h"
*/
