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
    ) {
        if ( etr ) {
            etr->listener.lock()->afterPulse( etr->whose );
        }
    }
}





template< class E >
inline void ListenerPlanet< E >::afterPulse( AEngine::Ptr whose ) {
    assert( engine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ������ � ������� � ����������
    assert( whose && "�� ������� ������, �������� �������." );
    auto engineStarSystem = std::static_pointer_cast< pes::Engine >( whose );


    // ������������ �������

    // ������������ ����� ��� / ����
    // # ������ �������� ������ ��������� ��� ����� ��������� ����
    //   ������� �������.
    // ������� ������� ������ � ������� ���������� ������ ������� �������
    const auto timedelta = engineStarSystem->timelive() - engine->timelive();
    if (timedelta <= 0) {
        // �������� � �������� ��� ������� �� ������������
        return;
    }

    // ������� ��� � ������� �������
    assert( (engine->timestep() > 0)
        && "��� ������� ��� ������� ������ ���� ������ 0." );
    const int pulsedelta =
        static_cast< int >( timedelta / engine->timestep() );
    // ����������, ����� ����� ������� ������� �������� ����������
    // ���� �� 1 ����� �������
    if (pulsedelta == 0) {
        return;
    }

    // ��������� ������� �� ����. �����
    *engine << pulsedelta;


    // ��� ����� ������� - �� ���������� ������
    // @see #���������� �� �������� ������� � starsystem::Listener.
}

                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
