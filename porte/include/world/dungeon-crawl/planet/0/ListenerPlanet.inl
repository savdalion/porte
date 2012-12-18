namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;




template< class E >
inline void ListenerPlanet< E >::notifyAfterPulse() {
    assert( engine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "����� ������� " <<
        engine->pulselive() <<
        " " << engine->timelive() << " s" <<
    std::endl;
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
    assert( engine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // # ������ ����� ����������� ������. �.�. ��������� �������� �������������
    //   ���������� ��� ������������� ������ ��� ��������.
    const bool notFirstPulse = (engine->pulselive() != 0);

    // ������������ ������ � ������� � ����������
    assert( whose && "�� ������� ������, �������� �������." );
    const auto engineStarSystem =
        std::static_pointer_cast< pes::Engine >( whose );

    // ������������ �������

    // ������������ ����� ��� / ����
    // # ������ �������� ������ ��������� ��� ����� ��������� ����
    //   ������� �������.
    // ������� ������� ������ � ������� ���������� ������ ������� �������
    const auto timedelta = engineStarSystem->timelive() - engine->timelive();
    if ( (timedelta <= 0) && notFirstPulse ) {
        // �������� � �������� ��� ������� �� ������������
        return;
    }

    // ������� ��� � ������� �������
    assert( (engine->timestep() > 0)
        && "��� ������� ��� ������� ������ ���� ������ 0." );
    const int pulsedelta =
        static_cast< int >( timedelta / engine->timestep() );
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
    *engine << ( (pulsedelta > 0) ? pulsedelta : 1 );


    // ��� ����� ������� - �� ���������� ������
    // @see #���������� �� �������� ������� � starsystem::Listener.
}





template< class E >
inline void ListenerPlanet< E >::updatePlanetFromStarSystem(
    const pns::topology_t&  topologyStarSystem
) {
    assert( engine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    auto& topology = engine->portulan()->topology().topology();

    // ��������� ���������
    // �����
    assert( (pnp::ILLUMINANCE_STAR_COUNT >= pns::STAR_COUNT)
        && "������ ��������� �� ����� �������� ��� ����� ������� �������. ��������� ILLUMINANCE_STAR_COUNT." );
    for (auto i = 0; i < pns::STAR_COUNT; ++i) {
        const pns::aboutStar_t& as = topologyStarSystem.star.content[ i ];
        /* - ������� ���������� ������ - ������������� ��� ������� �����
        assert( (as.mass >= 0.0)
            && "����� ������ �� ����� ���� ������ 0. ��������, ������ ��� ������������� ������ ����." );
        */
        if (as.mass <= 0) {
            // # ������ � ������ mass - ���� ������ ���.
            // ��������� ���� ������� - ������ radius
            static const pnp::aboutIlluminanceStar_t ais = {};
            topology.aboutIlluminanceSource.star[ i ] = ais;
            break;
        }
        const pnp::aboutIlluminanceStar_t ais = {
            as.radius,
            as.temperature,
            as.luminousIntensity,
            { as.coord[ 0 ],  as.coord[ 1 ],  as.coord[ 2 ] }
        };
        topology.aboutIlluminanceSource.star[ i ] = ais;

    } // for (auto i = 0; i < pns::STAR_COUNT; ++i)
}


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
