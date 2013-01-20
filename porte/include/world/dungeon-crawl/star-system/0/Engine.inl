namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


inline Engine::real_t Engine::extent() {
    // ����� ����� �������������� ����� - ��������� ��������
    if (mExtent > 0.0) {
        return mExtent;
    }

    // ������� ������������ ���������� �� ������ ��������� ����� ���������
    // ��������� ������� �������
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // ������ ���������� �� ������ � ������
    std::vector< real_t >  distance;

    const auto pushDistance = [ &distance ] (
        const real_t  coord[ 3 ]
   ) {
        const auto c = typelib::CoordT< real_t >( coord );
        distance.push_back( c.distance() );
    };

    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        const auto body = asteroid[ i ];
        if ( pns::presentAsteroid( &body ) ) {
            const real_t c[ 3 ] = {
                pns::coord1( &body.today.coord[ 0 ] ),
                pns::coord1( &body.today.coord[ 1 ] ),
                pns::coord1( &body.today.coord[ 2 ] )
            };
            pushDistance( c );
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

    // �.�. �������� ��������� ������ ������, ���� ��������� ����������
    mExtent = *std::max_element( distance.cbegin(), distance.cend() ) * 2.0;

    return mExtent;
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
