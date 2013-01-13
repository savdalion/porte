namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


template< class E >
inline void ListenerStarSystem< E >::notifyAndCompletePulse() {
    assert( mEngine &&
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    // # Просто заботимся, чтобы слушатели получили это событие.


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidCollisionStar() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutAsteroid_t&    aa = a[ ia ];
    const pns::aboutStar_t&  as = b[ ib ];

    // # Астероид уничтожен.
    // # Элемент может изменить только сам себя.
    // изменение массы отрабатывает сама звезда
    // здесь - только уничтожим астероид
    pns::excludeAsteroid( &aa );
    --delta.asteroid.count;


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidCollisionAsteroid() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    // # Это событие является обобщением других событий.
    //   См. Engine::dealEventCollision().


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidChangeTemperature() " <<
		a[ ia ].uid <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.temperature += deltaTemperature;


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidChangeVelocity() " <<
		a[ ia ].uid <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    aa.velocity[ 0 ] += deltaVelocity[ 0 ];
    aa.velocity[ 1 ] += deltaVelocity[ 1 ];
    aa.velocity[ 2 ] += deltaVelocity[ 2 ];


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );
    assert( (n > 1) &&
        "Количество осколков должно быть больше 1." );


    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventAsteroidCrushN() " <<
		a[ ia ].uid << " " << n << "x" <<
    std::endl;


    pns::aboutAsteroid_t&  aa = a[ ia ];

    // уничтожаем астероид
    pns::excludeAsteroid( &aa );
    --delta.asteroid.count;

    // на его месте создаём астероиды поменьше (осколки)
    // #! Нельзя запускать в параллельном режиме.

    // # Случайные генераторы привяжем к UID, чтобы облегчить отладку.
    static const auto SEED = static_cast< size_t >( aa.uid );
    typelib::Random< size_t >  seed( 0, 1000000000, SEED );
    typedef typelib::Random< pns::real_t >  randomReal_t;
    randomReal_t  genCoordX( 0.0,  aa.size[ 0 ],  seed.next() );
    randomReal_t  genCoordY( 0.0,  aa.size[ 1 ],  seed.next() );
    randomReal_t  genCoordZ( 0.0,  aa.size[ 2 ],  seed.next() );

    // радиусы осколков сделаем такими, чтобы общий объём осколков
    // был равен объёму астероида
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

    // скорости осколков "размажем" по общей скорости астероида
    // @todo fine Кинет. энергия должна сохраняться.
    const pns::real_t av = std::abs( typelib::average(
        aa.velocity[ 0 ],  aa.velocity[ 1 ],  aa.velocity[ 2 ]
    ) );
    const pns::real_t halfAV = typelib::empty( av ) ? 0.01 : (av / 2.0);
    randomReal_t  genDeltaVelocity( -halfAV,  halfAV,  seed.next() );


    // определим центры осколков астероида (астероидов поменьше)
    // и объёмы этих осколков
    std::vector< typelib::CoordT< pns::real_t > >  coordCenter;
    std::vector< pns::real_t >  volume;
    pns::real_t sumGenVolume = 0.0;

    // для каждой коорд. центра сгенерируем "базовые" радиусы, не превышающие
    // линейные размеры астероида (все осколки - в виде эллипсоидов, аналог.
    // астероидам)
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

    // нормализуем коэффициенты для объёмов, вычислим объёмы
    for (auto vtr = volume.begin(); vtr != volume.end(); ++vtr) {
        const pns::real_t ks = *vtr / sumGenVolume;
        *vtr = ks * allVolume;
    }

    assert( (coordCenter.size() == volume.size())
        && "Размер списков должен быть одинаков." );
    assert( (coordCenter.size() == baseRadius.size())
        && "Размер списков должен быть одинаков." );

    size_t freeI = pns::lastIndexOfPresentAsteroid( a ) + 1;
    pns::uid_t freeUID = pns::nextUIDAsteroid( a );
    auto ctr = coordCenter.cbegin();
    auto vtr = volume.cbegin();
    auto btr = baseRadius.cbegin();
    for ( ;  ctr != coordCenter.cend();  ++ctr, ++vtr, ++btr) {
        assert( (freeI < pns::ASTEROID_COUNT)
            && "Астероидов больше, чем выделено для них места." );

        // определим радиусы осколка, опираясь на сгенерированные
        // выше базовые радиусы
        const pns::real_t rx = btr->first;
        const pns::real_t ry = btr->second;
        // один из радиусов выровняем по объёму осколка
        const pns::real_t ve = *vtr;
        const pns::real_t rz =
            typelib::compute::geometry::ellipsoid::radiusByVolume( ve, rx, ry );

        const pns::real_t mass = ve * aa.density;
        const pns::real_t temperature = aa.temperature + deltaTemperature;

        const pns::real_t vx = aa.velocity[ 0 ] + genDeltaVelocity.next();
        const pns::real_t vy = aa.velocity[ 1 ] + genDeltaVelocity.next();
        const pns::real_t vz = aa.velocity[ 2 ] + genDeltaVelocity.next();

        // создадим осколок
        // # Осколок имеет тот же состав, что сам астероид.
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


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventPlanetCollisionStar() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutPlanet_t&      ap = a[ ia ];
    const pns::aboutStar_t&  as = b[ ib ];

    // # Звезда поглощает планету.
    //as.mass += ap.mass;
    pns::excludePlanet( &ap );
    --delta.planet.count;


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventStarCollisionStar() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutStar_t&        asA = a[ ia ];
    const pns::aboutStar_t&  asB = b[ ib ];

    // # @todo ? Звезда с большей массой поглощает звезду с меньшей.
    // # Звезда A поглощает звезду B.
    asA.mass += asB.mass;
    //pns::excludeStar( &asB );
    --delta.star.count;


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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
        "Движок не указан. Должны были позаботиться наследники." );

    // отрабатываем своё событие
    std::cout << "ListenerStarSystem::notifyAndCompleteEventStarCollisionAsteroid() " <<
		a[ ia ].uid << " + " << b[ ib ].uid <<
    std::endl;

    pns::aboutStar_t&            as = a[ ia ];
    const pns::aboutAsteroid_t&  aa = b[ ib ];

    // # Звезда поглощает астероид.
    // # Элемент может изменить только сам себя.
    // уничтожение астероида отрабатывает сам астероид
    // здесь - только изменим характеристики звезды
    as.mass += aa.mass;


    // отправляем *своё* событие другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
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

    // @todo Передать дальше.
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

    // @todo Передать дальше.
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

    // @todo Передать дальше.
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
