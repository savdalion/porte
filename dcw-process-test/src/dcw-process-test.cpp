#include "../include/StdAfx.h"
#include "../include/configure.h"

#include "../include/world/dangeon-crawl/star-system/AsteroidAsteroidCollision.h"
//#include "../include/world/dangeon-crawl/star-system/AsteroidStarCollision.h"


// ���������� � "porte" ��� ������������� �������� C++ ��� OpenCL.
#undef PORTULAN_AS_OPEN_CL_STRUCT

// (!) ������� ����������� define, �������� � ������ cl.h � wingdi.h, �.�.
// ��� ��������� � ������ ����� ��������.
// @todo bad solution
#undef CC_NONE
#undef CL_NONE




/**
* ������������ ��� ������� 'Porte', ��� 'dungeon-crawl'.
*/
int main( int argc, char** argv ) {

    using namespace porte::dcwtest;

    namespace tc = typelib::constant::physics;


    setlocale( LC_ALL, "Russian" );
    // ��� ����������� '.' ������ ','
    setlocale( LC_NUMERIC, "C" );




    ::testing::InitGoogleTest( &argc, argv );
    ::testing::FLAGS_gtest_death_test_style = "fast";
    int result = 0;
    try {
        result = RUN_ALL_TESTS();
    } catch ( ... ) {
        // @todo fine ��������� assert() � ��������� - ��������� ����:
        //       ��������� ����� ������ � �������. �������� assert'� ��
        //       exception?
    }




    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
