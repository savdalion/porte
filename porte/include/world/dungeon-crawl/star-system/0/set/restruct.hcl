/**
* (!) ��� ����������� �� ����� ����� ������ ���� ����������� ��
* ����������� C++.
*
*   # ��������� C++ � portulan ��������� ���, ����� �� ����������� ���.
*/

// ����������� ���� OpenCL, ����� ������������ ��������� �� C++
// �� ��������� if-def'���.

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


// #! ����� ������� �����������. ����� ��� ���������� ���� ������� ��������
//    ������������� ������... � ������� ��� ����-��������� ��������� �
//    ������� ������.
// #! ������ Unicode � ���� OpenCL (��������� �����, ��������, ����� �����������
//    ������� �� ���������) ��� ����, ���������� � Unicode, ����� ��������
//    � ������������� �������.
// # ������� ������� ��� ������� ������������� ������ > https://sourceforge.net/projects/clcc

// @see ���������� � helper.hcl.
