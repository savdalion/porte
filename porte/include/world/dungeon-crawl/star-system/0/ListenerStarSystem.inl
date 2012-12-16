namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


inline void ListenerStarSystem::afterPulse(
    pns::real_t timelive,
    const pns::topology_t& topology
) {
    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::afterPulse() " << timelive << " s" << std::endl;

    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) {
        etr->afterPulse( timelive, topology );
    }
}




inline void ListenerStarSystem::afterAsteroidCollisionStar(
    pns::asteroidContent_t a,  size_t ia,
    pns::starContent_t     b,  size_t ib,
    pns::deltaElement_t& delta
) {
    pns::aboutAsteroid_t& aa = a[ ia ];
    pns::aboutStar_t&     as = b[ ib ];

    // # Звезда поглощает астероид.
    as.mass += aa.mass;
    pns::excludeAsteroid( &aa );
    --delta.asteroid.count;
}




inline void ListenerStarSystem::afterChangeCountAsteroid(
    size_t current,  int delta
) {
#ifdef _DEBUG
    std::cout <<
        current << " asteroids (" << delta << ")" <<
    std::endl;
#endif
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
