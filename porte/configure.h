#pragma once

#include <string>


namespace porte {


/**
* ��� ������������� ����� �����, ���� OpenCL ����� ��������������� ��� ������
* ������� ����������. ��� ����� ����������� ��������� ������ ���������, ��
* �����������, ��� ��� ��������� ���� ���� ����� ������.
*
* ������� ����� ����� �����.
* ���� ���� �������� ��������� ������ (#include), OpenCL (�� ������� ���� ��
* NVidia) �� ��������� ��������� � ���. � ���������� ���������� �������������
* ������ ��� "��� �������� ����� ���".
*/
//#define ALWAYS_BUILD_CL_KERNEL_PORTE


static const std::string PATH_CL =
    "D:/Projects/workspace/porte/porte/cl-kernel";
static const std::string PATH_CL_DUNGEONCRAWL =
    PATH_CL + "/set/dungeon-crawl";



/**
* �������� ���� Dungeon Crawl, ������� ����� �������� � ������.
* �����������, ��� �������, ��� �������.
*   # ���������� ������� � ������������ ��������� ���� �� �����.
*/
#define COMPONENT_DUNGEONCRAWL_PORTE
#define LIVING_DUNGEONCRAWL_PORTE
#define TEMPERATURE_DUNGEONCRAWL_PORTE


} // porte
