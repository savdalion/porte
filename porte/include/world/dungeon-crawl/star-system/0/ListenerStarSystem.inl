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
