namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


template< class E >
inline void ListenerStarSystem< E >::notifyAfterPulse() {
    assert( engine &&
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
inline void ListenerStarSystem< E >::notifyAfterAsteroidCollisionStar(
    pns::asteroidContent_t a,  size_t ia,
    pns::starContent_t     b,  size_t ib,
    pns::deltaElement_t&   delta
) {
    assert( engine &&
        "������ �� ������. ������ ���� ������������ ����������." );

    // ������������ ��� �������
    std::cout << "ListenerStarSystem::notifyAfterAsteroidCollisionStar() " <<
    std::endl;

    pns::aboutAsteroid_t& aa = a[ ia ];
    pns::aboutStar_t&     as = b[ ib ];

    // # ������ ��������� ��������.
    as.mass += aa.mass;
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


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
