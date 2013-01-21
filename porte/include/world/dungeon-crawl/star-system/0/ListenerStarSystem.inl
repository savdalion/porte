namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


template< class E >
inline void ListenerStarSystem< E >::completePulse() {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    // # ������ ���������, ����� ��������� �������� ��� �������.

    /* - �������� �������� � Engine*::notify*CompleteEvent().
    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPulse(
        etr->whose
    ); } }
    */
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidCollisionStar(
    pns::asteroidContent_t a,  size_t ia,
    const pns::starContent_t b,  size_t ib
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventAsteroidCollisionStar() " <<
        a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    // # ��� ������� �������� ���������� ������ �������.
    //   ��. Engine::dealEventCollision().


    /* - �������� �������� � Engine*::notify*CompleteEvent().
    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidCollisionStar(
        etr->whose,  a, ia,  b, ib
    ); } }
    */
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidCollisionPlanet(
    pns::asteroidContent_t a,  size_t ia,
    const pns::planetContent_t b,  size_t ib
) {
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidCollisionAsteroid(
    pns::asteroidContent_t a,  size_t ia,
    const pns::asteroidContent_t b,  size_t ib
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventAsteroidCollisionAsteroid() " <<
        a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    // # ��� ������� �������� ���������� ������ �������.
    //   ��. Engine::dealEventCollision().


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidCollisionAsteroid(
        etr->whose,  a, ia,  b, ib
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidImpactForce(
    pns::asteroidContent_t a,  size_t ia,
    const pns::real_t force[ 3 ],  pns::real_t absForce
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventAsteroidImpactForce() " <<
    //    a[ ia ].uid << " " << absForce <<
    //std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    // # ��� ����� ������� ��� ������� ������������� �������.
    //   ��. Engine::*ImpactIn().
    aa.force[ 0 ] = force[ 0 ];
    aa.force[ 1 ] = force[ 1 ];
    aa.force[ 2 ] = force[ 2 ];
    aa.absForce = absForce;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidImpactForce(
        etr->whose,  a, ia,  force, absForce
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidChangeCoord(
    pns::asteroidContent_t a,  size_t ia,
    const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventAsteroidChangeCoord() " <<
    //    a[ ia ].uid << " " << absDeltaCoord <<
    //std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.coord[ 0 ] += deltaCoord[ 0 ];
    aa.coord[ 1 ] += deltaCoord[ 1 ];
    aa.coord[ 2 ] += deltaCoord[ 2 ];

    aa.deltaCoord[ 0 ] += deltaCoord[ 0 ];
    aa.deltaCoord[ 1 ] += deltaCoord[ 1 ];
    aa.deltaCoord[ 2 ] += deltaCoord[ 2 ];
    aa.absDeltaCoord = absDeltaCoord;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidChangeCoord(
        etr->whose,  a, ia,  deltaCoord, absDeltaCoord
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidChangeTemperature(
    pns::asteroidContent_t a,  size_t ia,
    const pns::real_t deltaTemperature
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventAsteroidChangeTemperature() " <<
        a[ ia ].uid <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.temperature += deltaTemperature;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidChangeTemperature(
        etr->whose,  a, ia,  deltaTemperature
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidChangeVelocity(
    pns::asteroidContent_t a,  size_t ia,
    const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventAsteroidChangeVelocity() " <<
    //    a[ ia ].uid << " " << absDeltaVelocity <<
    //std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.velocity[ 0 ] += deltaVelocity[ 0 ];
    aa.velocity[ 1 ] += deltaVelocity[ 1 ];
    aa.velocity[ 2 ] += deltaVelocity[ 2 ];

    aa.deltaVelocity[ 0 ] = deltaVelocity[ 0 ];
    aa.deltaVelocity[ 1 ] = deltaVelocity[ 1 ];
    aa.deltaVelocity[ 2 ] = deltaVelocity[ 2 ];
    aa.absDeltaVelocity = absDeltaVelocity;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidChangeVelocity(
        etr->whose,  a, ia,  deltaVelocity, absDeltaVelocity
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidCrushN(
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
    std::cout << "ListenerStarSystem::completeEventAsteroidCrushN() " <<
        a[ ia ].uid << " " << n << "x" <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    // ���������� ����� ��������
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
            { mass, 0.0 },
            // size
            { rx, ry, rz },
            // coord
            { ctr->x, ctr->y, ctr->z },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            0,
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
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidCrushN(
        etr->whose,  a, ia,  n,  deltaVelocity,  deltaTemperature
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventAsteroidDestroy(
    pns::asteroidContent_t a,  size_t ia,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventAsteroidDestroy() " <<
        a[ ia ].uid <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    // ���������� ��������
    pns::excludeAsteroid( &aa );
    --delta.asteroid.count;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidDestroy(
        etr->whose,  a, ia
    ); } }
}




template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetCollisionStar(
    pns::planetContent_t a,  size_t ia,
    const pns::starContent_t b,  size_t ib
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventPlanetCollisionStar() " <<
        a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    // # ��� ������� �������� ���������� ������ �������.
    //   ��. Engine::dealEventCollision().


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPlanetCollisionStar(
        etr->whose,  a, ia,  b, ib
    ); } }
}




template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetCollisionPlanet(
    pns::planetContent_t a,  size_t ia,
    const pns::planetContent_t b,  size_t ib
) {
}




template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetCollisionAsteroid(
    pns::planetContent_t a,  size_t ia,
    const pns::asteroidContent_t b,  size_t ib
) {
}





template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetImpactForce(
    pns::planetContent_t a,  size_t ia,
    const pns::real_t force[ 3 ],  pns::real_t absForce
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventPlanetImpactForce() " <<
    //    a[ ia ].uid << " " << absForce <<
    //std::endl;


    pns::aboutPlanet_t&  ap = a[ ia ];

    // # ��� ����� ������� ��� ������� ������������� �������.
    //   ��. Engine::*ImpactIn().
    ap.force[ 0 ] = force[ 0 ];
    ap.force[ 1 ] = force[ 1 ];
    ap.force[ 2 ] = force[ 2 ];
    ap.absForce = absForce;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPlanetImpactForce(
        etr->whose,  a, ia,  force, absForce
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetChangeCoord(
    pns::planetContent_t a,  size_t ia,
    const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventPlanetChangeCoord() " <<
    //    a[ ia ].uid << " " << absDeltaCoord <<
    //std::endl;


    pns::aboutPlanet_t&  ap = a[ ia ];

    ap.coord[ 0 ] += deltaCoord[ 0 ];
    ap.coord[ 1 ] += deltaCoord[ 1 ];
    ap.coord[ 2 ] += deltaCoord[ 2 ];

    ap.deltaCoord[ 0 ] += deltaCoord[ 0 ];
    ap.deltaCoord[ 1 ] += deltaCoord[ 1 ];
    ap.deltaCoord[ 2 ] += deltaCoord[ 2 ];
    ap.absDeltaCoord = absDeltaCoord;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPlanetChangeCoord(
        etr->whose,  a, ia,  deltaCoord, absDeltaCoord
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetChangeVelocity(
    pns::planetContent_t a,  size_t ia,
    const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventPlanetChangeVelocity() " <<
    //    a[ ia ].uid << " " << absDeltaVelocity <<
    //std::endl;


    pns::aboutPlanet_t&  ap = a[ ia ];

    ap.velocity[ 0 ] += deltaVelocity[ 0 ];
    ap.velocity[ 1 ] += deltaVelocity[ 1 ];
    ap.velocity[ 2 ] += deltaVelocity[ 2 ];

    ap.deltaVelocity[ 0 ] = deltaVelocity[ 0 ];
    ap.deltaVelocity[ 1 ] = deltaVelocity[ 1 ];
    ap.deltaVelocity[ 2 ] = deltaVelocity[ 2 ];
    ap.absDeltaVelocity = absDeltaVelocity;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPlanetChangeVelocity(
        etr->whose,  a, ia,  deltaVelocity, absDeltaVelocity
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventPlanetDestroy(
    pns::planetContent_t a,  size_t ia,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventPlanetDestroy() " <<
        a[ ia ].uid <<
    std::endl;


    pns::aboutPlanet_t&  ap = a[ ia ];

    // ���������� �������
    pns::excludePlanet( &ap );
    --delta.planet.count;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPlanetDestroy(
        etr->whose,  a, ia
    ); } }
}




template< class E >
inline void ListenerStarSystem< E >::completeEventStarCollisionStar(
    pns::starContent_t a,  size_t ia,
    const pns::starContent_t b,  size_t ib
) {
}




template< class E >
inline void ListenerStarSystem< E >::completeEventStarCollisionPlanet(
    pns::starContent_t a,  size_t ia,
    const pns::planetContent_t b,  size_t ib
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventStarCollisionPlanet() " <<
        a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    // # ��� ������� �������� ���������� ������ �������.
    //   ��. Engine::dealEventCollision().


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarCollisionPlanet(
        etr->whose,  a, ia,  b, ib
    ); } }
}




template< class E >
inline void ListenerStarSystem< E >::completeEventStarCollisionAsteroid(
    pns::starContent_t a,  size_t ia,
    const pns::asteroidContent_t b,  size_t ib
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventStarCollisionAsteroid() " <<
        a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutStar_t&            as = a[ ia ];
    const pns::aboutAsteroid_t&  aa = b[ ib ];

    // # ������ ��������� ��������.
    // # ������� ����� �������� ������ ��� ����.
    // ����������� ��������� ������������ ��� ��������
    // ����� - ������ ������� �������������� ������
    // # ����� ��������� ���� �� ��������� � ������ ������.
    as.mass.knoll += aa.mass.base + aa.mass.knoll;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarCollisionAsteroid(
        etr->whose,  a, ia,  b, ib
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventStarImpactForce(
    pns::starContent_t a,  size_t ia,
    const pns::real_t force[ 3 ],  pns::real_t absForce
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventStarImpactForce() " <<
    //    a[ ia ].uid << " " << absForce <<
    //std::endl;


    pns::aboutStar_t&  as = a[ ia ];

    // # ��� ����� ������� ��� ������� ������������� �������.
    //   ��. Engine::*ImpactIn().
    as.force[ 0 ] = force[ 0 ];
    as.force[ 1 ] = force[ 1 ];
    as.force[ 2 ] = force[ 2 ];
    as.absForce = absForce;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarImpactForce(
        etr->whose,  a, ia,  force, absForce
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventStarChangeCoord(
    pns::starContent_t a,  size_t ia,
    const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventStarChangeCoord() " <<
    //    a[ ia ].uid << " " << absDeltaCoord <<
    //std::endl;


    pns::aboutStar_t&  as = a[ ia ];

    as.coord[ 0 ] += deltaCoord[ 0 ];
    as.coord[ 1 ] += deltaCoord[ 1 ];
    as.coord[ 2 ] += deltaCoord[ 2 ];

    as.deltaCoord[ 0 ] += deltaCoord[ 0 ];
    as.deltaCoord[ 1 ] += deltaCoord[ 1 ];
    as.deltaCoord[ 2 ] += deltaCoord[ 2 ];
    as.absDeltaCoord = absDeltaCoord;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarChangeCoord(
        etr->whose,  a, ia,  deltaCoord, absDeltaCoord
    ); } }
}




template< class E >
inline void ListenerStarSystem< E >::completeEventStarChangeMass(
    pns::starContent_t a,  size_t ia,
    const pns::real4_t& deltaMass
) {
    // @todo fine ���������� ���� �������� / ������ / �������� �������
    //       ����� ������� > http://codenet.ru/progr/cpp/Macros.php
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventStarChangeMass() " <<
        a[ ia ].uid <<
    std::endl;


    pns::aboutStar_t&  as = a[ ia ];

    // ����� ����� ���� ����������� ������ ����� ������
    const auto ks = as.mass.base / deltaMass.base;
    if (ks > 1e9) {
        as.mass.knoll += deltaMass.base + deltaMass.knoll;
    } else {
        as.mass.base  += deltaMass.base;
        as.mass.knoll += deltaMass.knoll;
    }


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarChangeMass(
        etr->whose,  a, ia,  deltaMass
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventStarChangeVelocity(
    pns::starContent_t a,  size_t ia,
    const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    //std::cout << "ListenerStarSystem::completeEventStarChangeVelocity() " <<
    //    a[ ia ].uid << " " << absDeltaVelocity <<
    //std::endl;


    pns::aboutStar_t&  as = a[ ia ];

    as.velocity[ 0 ] += deltaVelocity[ 0 ];
    as.velocity[ 1 ] += deltaVelocity[ 1 ];
    as.velocity[ 2 ] += deltaVelocity[ 2 ];

    as.deltaVelocity[ 0 ] = deltaVelocity[ 0 ];
    as.deltaVelocity[ 1 ] = deltaVelocity[ 1 ];
    as.deltaVelocity[ 2 ] = deltaVelocity[ 2 ];
    as.absDeltaVelocity = absDeltaVelocity;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarChangeVelocity(
        etr->whose,  a, ia,  deltaVelocity, absDeltaVelocity
    ); } }
}





template< class E >
inline void ListenerStarSystem< E >::completeEventStarDestroy(
    pns::starContent_t a,  size_t ia,
    pns::deltaElement_t& delta
) {
    assert( mEngine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::completeEventStarDestroy() " <<
        a[ ia ].uid <<
    std::endl;


    pns::aboutStar_t&  as = a[ ia ];

    // ���������� ������
    pns::excludeStar( &as );
    --delta.star.count;


    // ���������� *���* ������� ������ ����������
    // @see #���������� �� �������� ����� ������� � starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterStarDestroy(
        etr->whose,  a, ia
    ); } }
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

    // @todo �������� ������. ����������� � notify*().
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

    // @todo �������� ������. ����������� � notify*().
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

    // @todo �������� ������. ����������� � notify*().
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
