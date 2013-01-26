// @require pragma.hcl


/**
* (!) ��� ����������� �� ����� ����� ������ ���� ����������� ��
* ����������� C++.
*
*   # ��������� C++ � portulan ��������� ���, ����� �� ����������� ���.
*/

// ����������� ���� OpenCL, ����� ������������ ��������� �� C++
// �� ��������� if-def'���.

typedef float   real_t;
typedef float2  real2_t;
typedef float3  real3_t;
typedef float4  real4_t;

typedef char    cl_char;
typedef uchar   cl_uchar;
typedef int     cl_int;
typedef uint    cl_uint;
typedef long    cl_long;
typedef ulong   cl_ulong;
typedef short   cl_short;
typedef ushort  cl_ushort;
typedef float   cl_float;
typedef float2  cl_float2;
typedef float4  cl_float4;

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
