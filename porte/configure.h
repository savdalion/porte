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


const std::string PATH_CL = "D:/Projects/workspace/porte/porte/cl-kernel";
const std::string PATH_CL_DUNGEONCRAWL = PATH_CL + "/set/dungeon-crawl";

} // porte
