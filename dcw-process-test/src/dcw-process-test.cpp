#include "../include/StdAfx.h"
#include "../include/configure.h"

#include "../include/world/dangeon-crawl/star-system/AsteroidAsteroidCollision.h"
//#include "../include/world/dangeon-crawl/star-system/AsteroidStarCollision.h"


// Включается в "porte" для интерпретации структур C++ как OpenCL.
#undef PORTULAN_AS_OPEN_CL_STRUCT

// (!) Убираем определения define, заданные в файлах cl.h и wingdi.h, т.к.
// они совпадают с именем наших структур.
// @todo bad solution
#undef CC_NONE
#undef CL_NONE




/**
* Тестирование для проекта 'Porte', мир 'dungeon-crawl'.
*/
int main( int argc, char** argv ) {

    using namespace porte::dcwtest;

    namespace tc = typelib::constant::physics;


    setlocale( LC_ALL, "Russian" );
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );




    ::testing::InitGoogleTest( &argc, argv );
    ::testing::FLAGS_gtest_death_test_style = "fast";
    int result = 0;
    try {
        result = RUN_ALL_TESTS();
    } catch ( ... ) {
        // @todo fine Встречает assert() в программе - закрывает окно:
        //       результат виден только в консоли. Заменить assert'ы на
        //       exception?
    }




    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
