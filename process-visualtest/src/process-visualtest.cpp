#include "../include/StdAfx.h"
#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>



/**
* ���������� ������������ ��� ������� 'Porte'.
* �������� �� �����.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace co = portulan::command;


    // ��������� ���������� ������ ���-�����
    axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_rarely );
    //axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_often );

    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> BEGIN" << std::endl;
    #ifdef _MSC_VER
        #ifdef _DEBUG
            EZLOGGERVLSTREAM( axter::log_always ) << "Debug." << std::endl;
        #else
            EZLOGGERVLSTREAM( axter::log_always ) << "Release." << std::endl;
        #endif
    #endif

            
    setlocale( LC_ALL, "Russian" );
    // ��� ����������� '.' ������ ','
    setlocale( LC_NUMERIC, "C" );



    const size_t GRID = 81;
    typedef portulan::Portulan< GRID, GRID, GRID >  plato_t;
    plato_t plato;


    // I. ������ ����� �� ����� �����.

    // ����������� �� �����
    //const std::string elevationFile = PATH_MEDIA + "a/center-plato-elevation-map.png";
    const std::string elevationFile = PATH_MEDIA + "a/minimax-elevation-map.png";
    // ������� �������� � ��
    // �������� "a/center-hill-elevation-map.png" �������������
    // = 500 � (������ ����������� = 400 ���)
    // ���������� ��������
    const size_t sizeImage = 400;
    const double scaleXY = 0.500 / static_cast< double>( sizeImage );
    // ������ = 50 �
    const double hMin = -0.010;
    const double hMax =  0.040;
    const double h = hMax - hMin;
    co::elevationMap( plato, "E", elevationFile, scaleXY, hMin, hMax );


    /* - @todo �������� ����� �����, ������� ����� �����
    const std::string waterFile = PATH_MEDIA + "a/water1-mask-map.png";
    // ��������� ������� � �������
    //const int gridHMin = plato_t::topology_t::signBitLayer_t::minCoord().z;
    const int gridHMin = -15;
    const int gridHMax = 0;
    co::flood( plato, "W", waterFile, gridHMin, gridHMax );
    */


#if 0
// - @todo ...
    // II. ���������� ������ ���������.
    // ����� - ���� ��������.
    typedef Process< GRID, GRID, GRID >  process_t;
    process_t process( &plato );


    // ��������� ��������
    // # ���������� ��������� �������� �.�., ����� ��� ����� ���� ����������
    //   � ������� OpenCL.

    // � ��� ���������� ���� (����� "Seed").
    // ���� ������������ � ���� ��� (���� �������, �����).
    // ����� �� ����� (����� "Earth"), ���� ������������ � ���.
    // �� ���������� �������, ��� ���������� (���������� ������ "Sapling").
    // ������ �����, ���� �� ���������� ������� (����� "Tree").
    // ������ ������������ ����� ������.
    // �� ���������� �������, ������ ������� (������ ������, ����� "DeadTree").

    // ���� ������������ � ���� ��� (���� �������, �����).
    {
        // ����� ��������� � ���������� ��������
        SingleProcess sp( "moveSaplingByForceField" );

        // ������� ������
        process << sp;
    }

#endif





    // III. ������� ���������.
    portulan::io::VolumeVTKVisual::option_t o;
    o[ "size-window" ] = 700;
    o[ "size-point" ] = 1;
    o[ "show-corner" ] = false;
    o[ "show-axes" ] = false;
    const auto color = typelib::json::Variant( "{ 'E': 'FFFFFFFF',  'C': '0000FFFF' }" );
    o[ "rgba" ] = color;
#if 0
    o[ "only" ] = ".temperature";
#endif

    portulan::io::VolumeVTKVisual visual( o );

    
    std::cout << std::endl << "�������� 'q' ��� ��������� ����������� � ������..." << std::endl << std::endl;

    const size_t PULSE = 1;
    size_t age = 0;
    while ( true ) {
        std::cout << "������� " << age << std::endl;

        visual << plato;

        std::cout << std::endl;

        visual.wait();

        // ��������� �������� ��� ��������
#if 1
        //chemistry << PULSE;
#else
        //chemistry( PULSE );
#endif

        age += PULSE;

    } // while




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
