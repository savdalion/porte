#pragma once

// ����� ����������� ����� ������� � ����������� ��������� ������������ ��������, ��� �������
#pragma warning( disable : 4503 )

// ���������� ��������� �������, ������� ���� �������� ��� ���������� � deprecated
// @see http://lists.boost.org/Archives/boost/2006/08/109338.php 
#pragma warning( disable : 4996 )

// @todo fine ���������� ���, ����� Warning C4251 ������ �� ��������.
// @source http://www.windows-api.com/microsoft/VC-Language/30952961/a-solution-to-warning-c4251--class-needs-to-have-dllinterface.aspx
#pragma warning( disable : 4251 )


// (!) ��������� �������������� ������� �������� ������ ���� ���. �����
//     ���������� � ��� ��� �������������� �����������.

// ����������� ������� �������������� "pcl::search::Search<PointT>::radiusSearch",
// ���������� ������ ����������� �� ��������� ���������������, ����� ���������
// ���������� ������ ��������������� const ��� volatile
//#pragma warning( disable : 4373 )

// �������������� double � int, �������� ������ ������
//#pragma warning( disable : 4244 )

// ����������� cl.hpp v 1.2
// C++ exception specification ignored except to indicate a function
// is not __declspec(nothrow)
#pragma warning( disable : 4290 )



// �������� ������� ������ � WinDef.h
#define NOMINMAX

// ���������� �������������� ���������
#define _USE_MATH_DEFINES


#include <assert.h>
#include <array>
#include <climits>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <typeinfo>
#include <windows.h>


#include <boost/lexical_cast.hpp>


// �������
#include <gtest/gtest.h>
