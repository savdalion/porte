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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "Пульс планеты " <<
        engine->pulselive() <<
        " " << engine->timelive() << " s" <<
    std::endl;
    // # Просто заботимся, чтобы слушатели получили это событие.


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // обеспечиваем доступ к движкам и портуланам
    assert( whose && "Не получен движок, которого слушают." );
    auto engineStarSystem = std::static_pointer_cast< pes::Engine >( whose );


    // отрабатываем событие

    // отрабатываем смену дня / ночи
    // # Должны получать верный результат при любом временном шаге
    //   звёздной системы.
    // сколько времени прошло с момента последнего пульса звёздной системы
    const auto timedelta = engineStarSystem->timelive() - engine->timelive();
    if (timedelta <= 0) {
        // холостой и обратный ход времени не отрабатываем
        return;
    }

    // сколько это в пульсах планеты
    assert( (engine->timestep() > 0)
        && "Шаг времени для планеты должен быть больше 0." );
    const int pulsedelta =
        static_cast< int >( timedelta / engine->timestep() );
    // дожидаемся, когда время звёздной системы позволит отработать
    // хотя бы 1 пульс планеты
    if (pulsedelta == 0) {
        return;
    }

    // переводим планету на след. пульс
    *engine << pulsedelta;


    // это чужое событие - не отправляем дальше
    // @see #Соглашение об отправке событий в starsystem::Listener.
}

                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
