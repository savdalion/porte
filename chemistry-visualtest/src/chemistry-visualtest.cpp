#include "../include/StdAfx.h"
#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>



/**
* ���������� ������������ ��� ������� 'Porte'.
* ���������� ����������� ������� (�������).
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace co = portulan::command::common;


    // ��������� ���������� ������ ���-�����
    axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_rarely );
    //axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_often );

    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / chemistry-visualtest -> BEGIN" << std::endl;
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

    // ���� �� �������� ����
    const std::string elevationFile = PATH_MEDIA + "a/center-plato-elevation-map.png";
    //const std::string elevationFile = PATH_MEDIA + "a/minimax-elevation-map.png";
    // ������� �������� � ��
    // �������� "a/center-hill-elevation-map.png" �������������
    // = 100 � (������ ����������� = 400 ���)
    // ���������� ��������
    const size_t sizeImage = 400;
    const double scaleXY = 0.100 / static_cast< double>( sizeImage );
    // ������ ����� = 50 �
    const double hMin = -0.010;
    const double hMax =  0.040;
    const double h = hMax - hMin;
    /* - �������� �����. ��. ����.
    city << co::elevationMap< GRID, GRID, GRID, float >( "NaCl", elevationFile, scaleXY, hMin, hMax );
    */
    co::elevationMap( plato, "NaCl", elevationFile, scaleXY, hMin, hMax );


    // �������� ����� �����, ������� ����� �����
    const std::string waterFile = PATH_MEDIA + "a/water1-mask-map.png";
    // ��������� ������� � �������
    //const int gridHMin = plato_t::topology_t::signBitLayer_t::minCoord().z;
    const int gridHMin = -15;
    const int gridHMax = 0;
    co::flood( plato, "H2O", waterFile, gridHMin, gridHMax );



    // II. ���������� ���������� ������.
    typedef Chemistry< GRID, GRID, GRID >  chemistry_t;
    chemistry_t chemistry( &plato );

    // ��������� ���. �������
    // NaCl(����) + H2O(����) =
    //   = Na+(������ ������, ������ ��� ���������) + Cl-(����� �����, ���� � ���������� ������) + H2O(����)

#if 0
// - �����������. ��������� ��� �������. ������� - ��. ����.

    /* - ���. ��������� ����� ������ �� �������. ��. ����.
    class DissolutionSalt: public ChemicalReaction {
        inline ...
    };
    chemistry << DissolutionSalt();
    */

    typedef ChemicalReaction< GRID, GRID, GRID >  chemicalReaction_t;
    chemicalReaction_t dissolutionSalt( plato );

    // ������� ��� ������ �������
    /* - ���������� �������� ����������? ��. ����.
    dissolutionSalt.need = boost::assign::list_of
        ("NaCl")
        ("H2O")
    ;
    */

    (?) �������� ������� ��������� (��. ����) �������� �������� �����������,
        �� �������� ����������� �������� ���������� ������������ ����������.
        ������� ���� ����� ����� �������������...

    (!) �������� - ��� ��������� ��������, �� �������� ���������� ��
        ������� ���������.

    (#) ���� ������������ �� ������� ������ ������ � �������������.

    // �������� ������ (�������� �������������, ����������) ��������� �����
    // ��������� �������� (���� ���. �������, ������� �������� ������ �����)
    // (!) �� ���� �� ������ ����������� � �����, �� � ������������; � �����
    // ������� ����������� - ���� �������� � ������...
    const auto mixing = [] ( const plato_t& env, const typelib::coordInt_t& ca, const typelib::coordInt_t& cb ) -> float {
        // ��� ������ ������� ���������� ����������, ����� ���� � ����
        // ���������� � �������� �������
        const bool need = ChemicalReaction::need( env, ca, cb, "NaCl", "H2O" );
        if ( !need ) {
            return 0.0f;
        }
        // ��������� ���-�� (%) NaCl � H2O ����������� �� 1 �����
        return 0.01f;
    };

    // ���������� �������, �� ����� �������
    /* - ������� ����� �� ���������� out-������.
    typedef std::pair< typelib::Sign<>, float >  signPercent_t;
    dissolutionSalt.out1 = [] () -> signPercent_t {
        return signPercent_t( ... );
    };
    */

    // ������� �� ���������� � �� �����������
    /* - ������� ����� �� ���������� heat-�����.
    dissolutionSalt.heat = [] () -> float {
        return 0.0f;
    };
    */

    chemistry << dissolutionSalt;

#endif


#if 0
// - @todo ...
    // ���. ������� ������� ������ ������: �.�. ����� ��������������

    /* - ������ �������. ��. ����.
    // ������� ������ �������
    // # ��������� ��� ������������
    reactionNaClH2O.in() << "NaCl" << "H2O";

    // �������� �������
    reactionNaClH2O.out() << "Na+" << "Cl-" << "H2O";

    // ����� ���-�� (%) NaCl ������������ � H2O �� 1 �����
    // (��� �������� ������������ ������� � �������� �������)
    // �����. ��������� ������������ (H2O):
    // [ ������ ����, ��������, ���, ������ ]
    // # �������� ����������� ��������� ������� ��������� ��� ���� ���������.
    const float speed[] = { 0.0f, 0.01f, 0.02f, 0.03f };
    reactionNaClH2O.speed( speed );

    // ��� ����� ���-�� (%) �������� �������� ���������� �������
    // (� 100 � H2O ����� ���������� 36 � NaCl ��� 20 C � 40 � ��� 100 C)
    // # ������������ ������� ��� �����, ��� �� ������������.
    const float satsol[] = { 0.0f, 0.36f, 0.40f, 0.50f };
    reactionNaClH2O.satsol( satsol );
    */

    // # �������, ��� ������� ������� �� ��������, �� ������������.
    //   ��������, ���� �� ����� ������������ � �������� ����.

    // (1) ������� ��� ����������� �� 0 �� 100 C
    {
        ChemicalReaction reaction;

        // ������� ������ �������
        // # ��������� ��� ������������
        //reactionNaClH2O.in() << "NaCl" << "H2O" << "0 < .temperature < 99";
        reaction << "NaCl" << "H2O";
        reaction.temperature( 0.0f, 99.0f );

        // �������� �������
        reaction >> "Na+" >> "Cl-" >> "H2O";

        // ����� ���-�� (%) NaCl ������������ � H2O �� 1 �����
        // (��� �������� ������������ ������� � �������� �������)
        // # �������� ����������� ��������� ������� ��������� ��� ���� ���������.
        reaction.speed( 0.01f );

        // ��� ����� ���-�� (%) �������� �������� ���������� �������
        // (� 100 � H2O ����� ���������� 36 � NaCl ��� 20 C � 40 � ��� 100 C)
        // # ������������ ������� ��� �����, ��� �� ������������.
        reaction.satsol( 0.36f );

        // ����� ���-�� ������� ���������� / ����������� ��� �������
        reaction.heat( 0.0f );

        // ������� ���. ������ �������
        chemistry << reaction;
    }


    // (2) ������� ��� ����������� ���� 0 C
    {
        ChemicalReaction reaction;

        reaction << "NaCl" << "H2O";
        reaction.temperature( -std::numeric_limits< float >::infinity(),  0.0f );
        /* - ������ ������ �� ����������, ��� �������
        reaction.out() << "Na+" << "Cl-" << "H2O";
        */
        reaction.speed ( 0.0f );
        reaction.satsol( 0.0f );
        reaction.heat  ( 0.0f );

        chemistry << reaction;
    }


    // (2) ������� ��� ����������� ���� 99 C
    {
        ChemicalReaction reaction;

        reaction << "NaCl" << "H2O";
        reaction.temperature( 99.0f,  std::numeric_limits< float >::infinity() );
        reaction >> "Na+" >> "Cl-" >> "H2O";
        reaction.speed ( 0.02f );
        reaction.satsol( 0.40f );
        reaction.heat  ( 0.0f  );

        chemistry << reaction;
    }

#endif






    // III. ������� ���������.
    portulan::io::VolumeVTKVisual::option_t o;
    o[ "size-window" ] = 700;
    o[ "size-point" ] = 1;
    o[ "show-corner" ] = false;
    o[ "show-axes" ] = false;
    const auto color = typelib::json::Variant( "{ 'NaCl': 'FFFFFFFF',  'H2O': '0000FFFF' }" );
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




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / chemistry-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
