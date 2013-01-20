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

/* - @see pragma.hcl
// ���, �� ��� ���������� ������������ 'double' ��� OpenCL.
// #! ���� ������ ������ ���� ����������� � OpenCL HOST.
// # double4 �������������� �� ����� ������������ OpenCL. �� ����������.
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


// #! ����� ������� �����������. ����� ��� ���������� ���� ������� ��������
//    ������������� ������... � ������� ��� ����-��������� ��������� �
//    ������� ������.
// #! ������ Unicode � ���� OpenCL (�������� �����, ��������, ����� �����������
//    ������� �� ���������) ��� ����, ���������� � Unicode, ����� ��������
//    � ������������� �������.
// # ������� ������� ��� ������� ������������� ������ > https://sourceforge.net/projects/clcc

// @see ���������� � helper.hcl.


// # Include-����� ���������� � ���� C++ ��� ������ ����.
