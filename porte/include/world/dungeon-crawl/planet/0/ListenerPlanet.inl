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

    /* - @todo отрабатываем смену дн€ / ночи
    // # ƒолжны получать одинаковый результат при любом временном шаге
    //   звЄздной системы.
    // сколько времени прошло с момента последнего пульса звЄздной системы
    const auto timedelta = timelive - timecomplete;
    if (timedelta <= 0) {
        // обратный ход времени и холостой не отрабатываем
        return;
    }

    // сколько это в пульсах планеты
    const int pulsedelta = static_cast< int >( timedelta / timestep );
    */


    // чужое событие - не отправл€ем дальше
    // @see #—оглашение об отправке чужих событий в starsystem::Listener.
}

                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
