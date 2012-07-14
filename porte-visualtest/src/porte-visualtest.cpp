#include "../include/StdAfx.h"
#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>



/**
* ���������� ������������ ��� ������� 'Porte'.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace co = portulan::command;


    // ��������� ���������� ������ ���-�����
    axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_rarely );
    //axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_often );

    EZLOGGERVLSTREAM( axter::log_always ) << "Porte -> BEGIN" << std::endl;
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
    typedef portulan::Portulan3D< GRID, GRID, GRID >  city_t;
    city_t city;


    // I. ������ ����� ����������.
    
    const co::fnTemperature_t& fnt = [] ( const typelib::coordInt_t& c ) -> float {
        // @test ���������� ����������� �� ���� �������
        //return 20.0f;

        // @test ����������� ������� �� ������ Z
        //return -static_cast< float >( c.z ) * 10.0f;

        // @test ����������� ������� �� �������� � ������
        return 4800.0f - static_cast< float >( c.x * c.x + c.y * c.y + c.z * c.z );
    };

    co::temperature( city, fnt );


    // II. ���������� ������ �����������.
    typedef HeatTransfer< GRID, GRID, GRID >  heatTransfer_t;
    heatTransfer_t heatTransfer;

    
    // II. ������� ���������.
    portulan::io::VolumeVTKVisual::option_t o;
    o[ "size-window" ] = 700;
    o[ "size-point" ] = 1;
    o[ "show-corner" ] = false;
    o[ "show-axes" ] = false;
    o[ "only" ] = ".temperature";

    portulan::io::VolumeVTKVisual visual( o );

    while ( true ) {
        visual << city;
        visual.wait();

        // �������� ��� ��������: ������ - �����, ������ - �����
#if 1
        heatTransfer >> city;
#else
        heatTransfer( city, 1 );
#endif

    } // while




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
