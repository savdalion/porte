namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


inline void ListenerPlanet::afterPulse(
    pns::real_t timelive,
    const pns::topology_t& topology
) {
    std::cout << "ListenerPlanet::afterPulse() " << timelive << " s" << std::endl;

    /* - @todo ������������ ����� ��� / ����
    // # ������ �������� ���������� ��������� ��� ����� ��������� ����
    //   ������� �������.
    // ������� ������� ������ � ������� ���������� ������ ������� �������
    const auto timedelta = timelive - timecomplete;
    if (timedelta <= 0) {
        // �������� ��� ������� � �������� �� ������������
        return;
    }

    // ������� ��� � ������� �������
    const int pulsedelta = static_cast< int >( timedelta / timestep );
    */


    // ����� ������� - �� ���������� ������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
}

                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
