namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pniop = portulan::io::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;


template< class E >
inline void ListenerPlanet< E >::notifyAndCompletePulse() {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

#ifdef _DEBUG
    // @test
    std::cout << "����� ������� " << mEngine->live() << std::endl;

    // �������������
    /* - @todo
    pniop::StatOgre3DVisual::option_t  o;
    o[ "source-options" ] = "visual-planet.json";
    pniop::StatOgre3DVisual  visual( o );
    engine->portulan()->draw( visual );
    */
#endif

    // # ������ ���������, ����� ��������� �������� ��� �������.

    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerPlanet::begin();
         etr; etr = StoreListenerPlanet::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterPulse( etr->whose );
    } }
}





template< class E >
inline void ListenerPlanet< E >::afterPulse( AEngine::Ptr whose ) {

    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // # ������ ����� ����������� ������. �.�. ��������� �������� �������������
    //   ���������� ��� ������������� ������ ��� ��������.
    const bool notFirstPulse = !mEngine->live().first();

    // ������������ ������ � ������� � ����������
    assert( whose && "�� ������� ������, �������� �������." );
    const auto engineStarSystem =
        std::static_pointer_cast< pes::Engine >( whose );

    // ������������ �������

    // ������������ ����� ��� / ����
    // # ������ �������� ������ ��������� ��� ����� ��������� ����
    //   ������� �������.
    // ������� ������� ������ � ������� ���������� ������ ������� �������
    const auto timedelta =
        engineStarSystem->live().timelive() - mEngine->live().timelive();
    if ( (timedelta <= 0) && notFirstPulse ) {
        // �������� � �������� ��� ������� �� ������������
        return;
    }

    // ������� ��� � ������� �������
    assert( (mEngine->timestep() > 0)
        && "��� ������� ��� ������� ������ ���� ������ 0." );
    const auto pulsedelta =
        Pulse::pulsedelta( timedelta, mEngine->timestep() );
    // ����������, ����� ����� ������� ������� �������� ����������
    // ���� �� 1 ����� ������� (���� ��� �� 1-� �����)
    if ( (pulsedelta == 0) && notFirstPulse ) {
        return;
    }

    // ��������� �������������� ��������� �������
    const auto& topologyStarSystem =
        engineStarSystem->portulan()->topology().topology();
    updatePlanetFromStarSystem( topologyStarSystem );

    // ��������� �� ������� ������� �������������� ���������,
    // ��������� ������� �� ����. �����
    // # ������ �� ����� ���������� �������.
    // ������������� �� ������� ������� ������� ����
    *mEngine << ( (pulsedelta > 0) ? pulsedelta : 1 );


    // ��� ����� ������� - �� ���������� ������
    // @see #���������� �� �������� ������� � starsystem::Listener.
}





template< class E >
inline void ListenerPlanet< E >::updatePlanetFromStarSystem(
    const pns::topology_t&  topologyStarSystem
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    auto& topology = mEngine->portulan()->topology().topology();

    // ��������� ���������
    // �����
    assert( (pnp::ILLUMINANCE_STAR_COUNT >= pns::STAR_COUNT)
        && "������ ��������� �� ����� �������� ��� ����� ������� �������. ��������� ILLUMINANCE_STAR_COUNT." );
    static const pnp::aboutIlluminanceStar_t ZERO_AIS = {};
    pnp::aboutIlluminanceStar_t* ais = topology.aboutIlluminanceSource.star;
    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        const pns::aboutStar_t& as = topologyStarSystem.star.content[ i ];
        /* - ������� ���������� ������ - ������������� ��� ������� �����
        assert( (as.mass >= 0.0)
            && "����� ������ �� ����� ���� ������ 0. ��������, ������ ��� ������������� ������ ����." );
        */
        if ( pns::absentStar( as ) ) {
            // # ���� ������ ���.
            // ��������� ���� ������� - ������ radius
            ais[ i ] = ZERO_AIS;
            break;
        }
        const pnp::aboutIlluminanceStar_t newAIS = {
            static_cast< cl_float >( as.radius ),
            static_cast< cl_float >( as.surfaceTemperature ),
            static_cast< cl_float >( as.luminosity ),
            {
                static_cast< cl_float >( as.coord[ 0 ] ),
                static_cast< cl_float >( as.coord[ 1 ] ),
                static_cast< cl_float >( as.coord[ 2 ] )
            }
        };
        ais[ i ] = newAIS;

    } // for (auto i = 0; i < pns::STAR_COUNT; ++i)
}


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
