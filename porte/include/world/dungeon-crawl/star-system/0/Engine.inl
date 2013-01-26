namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


inline Engine::real_t Engine::extent() {
    // метод может использоваться часто - результат кешируем
    if (mExtent > 0.0) {
        return mExtent;
    }

    // находим максимальное расстояние от центра координат среди элементов
    // портулана звёздной системы
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // соберём расстояния от центра в список
    std::vector< real_t >  distance;

    const auto pushDistance = [ &distance ] (
        const pns::real3_t& coord
    ) {
        const auto c = typelib::CoordT< real_t >(
            coord.s[ 0 ],  coord.s[ 1 ],  coord.s[ 2 ]
        );
        distance.push_back( c.distance() );
    };

    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        const auto body = asteroid[ i ];
        if ( pns::presentAsteroid( &body ) ) {
            pushDistance( body.today.coord );
        }
    }

    for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
        const auto body = planet[ i ];
        if ( pns::presentPlanet( &body ) ) {
            pushDistance( body.today.coord );
        }
    }

    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        const auto body = star[ i ];
        if ( pns::presentStar( &body ) ) {
            pushDistance( body.today.coord );
        }
    }

    // т.к. элементы вращаются вокруг центра, берём удвоенное расстояние
    mExtent = *std::max_element( distance.cbegin(), distance.cend() ) * 2.0;

    return mExtent;
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
