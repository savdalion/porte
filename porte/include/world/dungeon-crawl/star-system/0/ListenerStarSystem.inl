namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


template< class E >
inline void ListenerStarSystem< E >::notifyAndCompletePulse() {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    // # ������ ���������, ����� ��������� �������� ��� �������.


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterPulse( etr->whose );
    } }

}





template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventAsteroidCollisionStar(
    pns::asteroidContent_t a,  size_t ia,
    const pns::starContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidCollisionStar() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutAsteroid_t&    aa = a[ ia ];
    const pns::aboutStar_t&  as = b[ ib ];

    // # �������� ���������.
    // # ������� ����� �������� ������ ��� ����.
    // ��������� ����� ������������ ���� ������
    // ����� - ������ ��������� ��������
    pns::excludeAsteroid( &aa );
    --delta.asteroid.count;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterAsteroidCollisionStar(
            etr->whose,  a, ia,  b, ib,  delta
        );
    } }
}





template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventAsteroidCollisionPlanet(
    pns::asteroidContent_t a,  size_t ia,
    const pns::planetContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
}





template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventAsteroidCollisionAsteroid(
    pns::asteroidContent_t a,  size_t ia,
    const pns::asteroidContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidCollisionAsteroid() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    // # ��� ������� �������� ���������� ������ �������.
    //   ��. Engine::dealEventCollision().


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterAsteroidCollisionAsteroid(
            etr->whose,  a, ia,  b, ib,  delta
        );
    } }
}





template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventAsteroidChangeTemperature(
    pns::asteroidContent_t a,  size_t ia,
    const pns::real_t deltaTemperature
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidChangeTemperature() " <<
		a[ ia ].uid <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.temperature += deltaTemperature;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterAsteroidChangeTemperature(
            etr->whose,  a, ia,  deltaTemperature
        );
    } }
}





template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventAsteroidChangeVelocity(
    pns::asteroidContent_t a,  size_t ia,
    const pns::real_t deltaVelocity[ 3 ]
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidChangeVelocity() " <<
		a[ ia ].uid <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.velocity[ 0 ] += deltaVelocity[ 0 ];
    aa.velocity[ 1 ] += deltaVelocity[ 1 ];
    aa.velocity[ 2 ] += deltaVelocity[ 2 ];


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterAsteroidChangeVelocity(
            etr->whose,  a, ia,  deltaVelocity
        );
    } }
}





template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventAsteroidCrushN(
    pns::asteroidContent_t a,  size_t ia,
    pns::deltaElement_t& delta,
    size_t n,
    const pns::real_t  deltaVelocity[ 3 ],
    const pns::real_t  deltaTemperature
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );
    assert( (n > 1) &&
        "���������� �������� ������ ���� ������ 1." );


    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidCrushN() " <<
		a[ ia ].uid << " " << n << "x" <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    // ���������� ��������
    pns::excludeAsteroid( &aa );
    --delta.asteroid.count;

    // �� ��� ����� ������ ��������� �������� (�������)
    // #! ������ ��������� � ������������ ������.

    // # ��������� ���������� �������� � UID, ����� ��������� �������.
    static const auto SEED = static_cast< size_t >( aa.uid );
    typelib::Random< size_t >  seed( 0, 1000000000, SEED );
    typedef typelib::Random< pns::real_t >  randomReal_t;
    randomReal_t  genCoordX( 0.0,  aa.size[ 0 ],  seed.next() );
    randomReal_t  genCoordY( 0.0,  aa.size[ 1 ],  seed.next() );
    randomReal_t  genCoordZ( 0.0,  aa.size[ 2 ],  seed.next() );

    // ������� �������� ������� ������, ����� ����� ����� ��������
    // ��� ����� ������ ���������
    const pns::real_t allVolume = typelib::compute::geometry::ellipsoid::volume(
        aa.size[ 0 ],  aa.size[ 1 ],  aa.size[ 2 ]
    );

    static const pns::real_t LEGAL_MIN_SIZE = 0.1;

    pns::real_t minSize =
        typelib::min( aa.size[ 0 ],  aa.size[ 1 ],  aa.size[ 2 ] );
    if (minSize < LEGAL_MIN_SIZE) { minSize = LEGAL_MIN_SIZE; }
    const pns::real_t minVolume = typelib::compute::geometry::ellipsoid::volume(
        minSize, minSize, minSize
    );
    randomReal_t  genVolume(
        minVolume / static_cast< pns::real_t >( n ),
        allVolume / static_cast< pns::real_t >( n ),
        seed.next()
    );

    // �������� �������� "��������" �� ����� �������� ���������
    // @todo fine �����. ������� ������ �����������.
    const pns::real_t av = std::abs( typelib::average(
        aa.velocity[ 0 ],  aa.velocity[ 1 ],  aa.velocity[ 2 ]
    ) );
    const pns::real_t halfAV = typelib::empty( av ) ? 0.01 : (av / 2.0);
    randomReal_t  genDeltaVelocity( -halfAV,  halfAV,  seed.next() );


    // ��������� ������ �������� ��������� (���������� ��������)
    // � ������ ���� ��������
    std::vector< typelib::CoordT< pns::real_t > >  coordCenter;
    std::vector< pns::real_t >  volume;
    pns::real_t sumGenVolume = 0.0;

    // ��� ������ �����. ������ ����������� "�������" �������, �� �����������
    // �������� ������� ��������� (��� ������� - � ���� �����������, ������.
    // ����������)
    typedef std::pair< pns::real_t, pns::real_t >  baseRadius_t;
    std::vector< baseRadius_t >  baseRadius;
    pns::real_t maxSize =
        typelib::max( aa.size[ 0 ],  aa.size[ 1 ],  aa.size[ 2 ] );
    if (maxSize < LEGAL_MIN_SIZE) { maxSize = LEGAL_MIN_SIZE * 2.0; }
    randomReal_t  genBaseRadius(
        minSize / static_cast< pns::real_t >( n ),  maxSize,  seed.next()
    );

    for (size_t k = 0; k < n; ++k) {
        const typelib::CoordT< pns::real_t >  coord(
            genCoordX.next(),  genCoordY.next(),  genCoordZ.next()
        );
        coordCenter.push_back( coord );

        const pns::real_t gv = genVolume.next();
        volume.push_back( gv );
        sumGenVolume += gv;

        baseRadius.push_back( std::make_pair(
            genBaseRadius.next(), genBaseRadius.next()
        ) );
    }

    // ����������� ������������ ��� �������, �������� ������
    for (auto vtr = volume.begin(); vtr != volume.end(); ++vtr) {
        const pns::real_t ks = *vtr / sumGenVolume;
        *vtr = ks * allVolume;
    }

    assert( (coordCenter.size() == volume.size())
        && "������ ������� ������ ���� ��������." );
    assert( (coordCenter.size() == baseRadius.size())
        && "������ ������� ������ ���� ��������." );

    size_t freeI = pns::lastIndexOfPresentAsteroid( a ) + 1;
    pns::uid_t freeUID = pns::nextUIDAsteroid( a );
    auto ctr = coordCenter.cbegin();
    auto vtr = volume.cbegin();
    auto btr = baseRadius.cbegin();
    for ( ;  ctr != coordCenter.cend();  ++ctr, ++vtr, ++btr) {
        assert( (freeI < pns::ASTEROID_COUNT)
            && "���������� ������, ��� �������� ��� ��� �����." );

        // ��������� ������� �������, �������� �� ���������������
        // ���� ������� �������
        const pns::real_t rx = btr->first;
        const pns::real_t ry = btr->second;
        // ���� �� �������� ��������� �� ������ �������
        const pns::real_t ve = *vtr;
        const pns::real_t rz =
            typelib::compute::geometry::ellipsoid::radiusByVolume( ve, rx, ry );

        const pns::real_t mass = ve * aa.density;
        const pns::real_t temperature = aa.temperature + deltaTemperature;

        const pns::real_t vx = aa.velocity[ 0 ] + genDeltaVelocity.next();
        const pns::real_t vy = aa.velocity[ 1 ] + genDeltaVelocity.next();
        const pns::real_t vz = aa.velocity[ 2 ] + genDeltaVelocity.next();

        // �������� �������
        // # ������� ����� ��� �� ������, ��� ��� ��������.
        const pns::aboutAsteroid_t asteroid = {
            // uid
            freeUID,
            // live
            true,
            // mass
            mass,
            // size
            { rx, ry, rz },
            // coord
            { ctr->x, ctr->y, ctr->z },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { vx, vy, vz },
            // density
            aa.density,
            // temperature
            temperature,
            // albedo
            aa.albedo,
            // meltingPoint
            aa.meltingPoint,
            // boilingPoint
            aa.boilingPoint,
            // heatCapacity
            aa.heatCapacity,
            // enthalpyFusion
            aa.enthalpyFusion,
            // enthalpyVaporization
            aa.enthalpyVaporization,
            // # memoryEvent
            { 0, {} }
        };

        a[ freeI ] = asteroid;

        ++freeI;
        ++freeUID;
    }


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterAsteroidCrushN(
            etr->whose,  a, ia,  delta,  n,  deltaVelocity,  deltaTemperature
        );
    } }
}




template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventPlanetCollisionStar(
    pns::planetContent_t a,  size_t ia,
    const pns::starContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventPlanetCollisionStar() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutPlanet_t&      ap = a[ ia ];
    const pns::aboutStar_t&  as = b[ ib ];

    // # ������ ��������� �������.
    //as.mass += ap.mass;
    pns::excludePlanet( &ap );
    --delta.planet.count;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterPlanetCollisionStar(
            etr->whose,  a, ia,  b, ib,  delta
        );
    } }
}




template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventPlanetCollisionPlanet(
    pns::planetContent_t a,  size_t ia,
    const pns::planetContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
}




template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventPlanetCollisionAsteroid(
    pns::planetContent_t a,  size_t ia,
    const pns::asteroidContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
}




template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventStarCollisionStar(
    pns::starContent_t a,  size_t ia,
    const pns::starContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventStarCollisionStar() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutStar_t&        asA = a[ ia ];
    const pns::aboutStar_t&  asB = b[ ib ];

    // # @todo ? ������ � ������� ������ ��������� ������ � �������.
    // # ������ A ��������� ������ B.
    asA.mass += asB.mass;
    //pns::excludeStar( &asB );
    --delta.star.count;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterStarCollisionStar(
            etr->whose,  a, ia,  b, ib,  delta
        );
    } }
}




template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventStarCollisionPlanet(
    pns::starContent_t a,  size_t ia,
    const pns::planetContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
}




template< class E >
inline void ListenerStarSystem< E >::notifyAndCompleteEventStarCollisionAsteroid(
    pns::starContent_t a,  size_t ia,
    const pns::asteroidContent_t b,  size_t ib,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAndCompleteEventStarCollisionAsteroid() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutStar_t&            as = a[ ia ];
    const pns::aboutAsteroid_t&  aa = b[ ib ];

    // # ������ ��������� ��������.
    // # ������� ����� �������� ������ ��� ����.
    // ����������� ��������� ������������ ��� ��������
    // ����� - ������ ������� �������������� ������
    as.mass += aa.mass;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) {
        etr->listener.lock()->afterStarCollisionAsteroid(
            etr->whose,  a, ia,  b, ib,  delta
        );
    } }
}




template< class E >
inline void ListenerStarSystem< E >::afterChangeCountAsteroid(
    size_t current,  int delta
) {
#ifdef _DEBUG
    std::cout <<
        current << " asteroids (" << delta << ")" <<
    std::endl;
#endif

    // @todo �������� ������.
}





template< class E >
inline void ListenerStarSystem< E >::afterChangeCountPlanet(
    size_t current,  int delta
) {
#ifdef _DEBUG
    std::cout <<
        current << " planets (" << delta << ")" <<
    std::endl;
#endif

    // @todo �������� ������.
}





template< class E >
inline void ListenerStarSystem< E >::afterChangeCountStar(
    size_t current,  int delta
) {
#ifdef _DEBUG
    std::cout <<
        current << " stars (" << delta << ")" <<
    std::endl;
#endif

    // @todo �������� ������.
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
