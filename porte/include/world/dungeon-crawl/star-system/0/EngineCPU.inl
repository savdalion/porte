namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;



inline EngineCPU::EngineCPU(
    real_t timestep
) :
    EngineWithoutBooster< pns::Portulan, pns::real_t > ( timestep )
{
    static const pns::statistics_t S_EMPTY = {
        // distance
        {
            std::numeric_limits< real_t >::max(),
            std::numeric_limits< real_t >::min()
        },
        // velocity
        {
            std::numeric_limits< real_t >::max(),
            std::numeric_limits< real_t >::min()
        }
    };
    mStatistics = S_EMPTY;


    // # Отдаём ссылку на себя слушателю.
    ListenerStarSystem::mEngine = this;
}





inline EngineCPU::~EngineCPU() {
}




inline EngineCPU::real_t EngineCPU::timeConverge() {
    harvestStatisticsForOptimizeTimestep();
    const real_t eta = mStatistics.estimateTimeApproaching[ 0 ];
    assert( typelib::empty( eta ) && "Статистика не собрана." );

    return eta;
}





inline void EngineCPU::pulse( int n ) {
    // выполняем 'n' циклов
    for (int i = 0; i < n; ++i) {
        pulse();
    }

    // вычисляем оптим. шаг времени, *предполагая*, что кол-во
    // пульсов будет тем же
    /* - @todo Время получается слишком малым. Разобраться.
    calcOptimalTimestep( n );
    */
}






inline void EngineCPU::pulse() {

    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& observer = topology.observer;
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // рассчитываем воздействия на тела звёздной системы
    // # Реализуем т.о., чтобы минимальными усилиями перенести на OpenCL.
    //   Для этого (1) запомним события с двумя и более участниками в
    //   памяти наблюдателя (observer_t), чтобы (2) после уведомить всех
    //   участников события, а потом (3) корректно и *независимо* отработать
    //   события каждым участником.

    // (1) запомним события с двумя и более участниками в памяти наблюдателя

    // воздействие на астероиды
    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        auto& e = asteroid[ i ];
        if ( pns::absentAsteroid( e ) ) {
            break;
        }
        asteroidImpactIn( &e, i );
    }

    // воздействие на планеты
    for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
        auto& e = planet[ i ];
        if ( pns::absentPlanet( e ) ) {
            break;
        }
        planetImpactIn( &e, i );
    }

    // воздействие на звёзды
    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        auto& e = star[ i ];
        if ( pns::absentStar( e ) ) {
            break;
        }
        starImpactIn( &e, i );
    }


    // (2) уведомим всех участников событий
    dealEvent();


    // (3) корректно и *независимо* отработаем события каждым участником,
    // уведомляем слушателей
    notifyAndCompleteEvent();


    // мир становится старше
    mLive.inc( mTimestep );

    // пульс пройден
    notifyAndCompletePulse();


    // собираем статистику для элементов портулана
    statistics();
}





inline void EngineCPU::asteroidImpactIn(
    pns::aboutAsteroid_t* aa,
    size_t currentI
) {
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& observer = topology.observer;
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // рассчитаем результирующую силу
    real_t force[ 3 ] = { 0, 0, 0 };
    pns::aboutBody_t a = {
        aa->mass,
        { aa->coord[ 0 ],    aa->coord[ 1 ],    aa->coord[ 2 ] },
        { aa->rotation[ 0 ], aa->rotation[ 1 ], aa->rotation[ 2 ] }
    };

    const real_t maxSide =
        typelib::max( aa->size[ 0 ], aa->size[ 1 ], aa->size[ 2 ] );


    // действие астероидов
    // # Слишком малы, чтобы гравитационно воздействовать на себя.
#if 0
    // Но вполне могут столкнуться друг с другом.
    for (size_t k = 0; k < pns::ASTEROID_COUNT; ++k) {
        if (currentI == k) {
            // сам на себя не действует
            continue;
        }
        const pns::aboutAsteroid_t& aak = asteroid[ k ];
        if ( pns::absent( aak ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const real_t maxSideK =
            typelib::max( aak.size[ 0 ], aak.size[ 1 ], aak.size[ 2 ] );
        const real_t collisionDistance = std::max( maxSide, maxSideK );
        if ( collision( aa->coord, aak.coord, collisionDistance ) ) {
            memorizeNext(
                pns::GE_ASTEROID, aa->event, &countEvent,
                pns::E_COLLISION_ASTEROID, k
            );
        }
    }
#endif


    // действие планет
    for (size_t k = 0; k < pns::PLANET_COUNT; ++k) {
        // силы гравитации, действующие на астероид со стороны
        // планеты с индексом 'k'
        const pns::aboutPlanet_t& apk = planet[ k ];
        if ( pns::absentPlanet( apk ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const pns::aboutBody_t b = {
            apk.mass,
            { apk.coord[ 0 ],    apk.coord[ 1 ],    apk.coord[ 2 ] },
            { apk.rotation[ 0 ], apk.rotation[ 1 ], apk.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( maxSide, apk.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::event_t event = {
                // uid события
                pns::E_COLLISION,
                // pi второй участник события
                { pns::GE_PLANET, k, apk.uid }
            };
            pns::asteroidMemorizeEvent( &aa->memoryEvent, event );
#ifdef _DEBUG
            pns::printEvent( pns::GE_ASTEROID, currentI, event, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    // действие звёзд
    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        // силы гравитации, действующие на астероид со стороны
        // звезды с индексом 'k'
        const pns::aboutStar_t& ask = star[ k ];
        if ( pns::absentStar( ask ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const pns::aboutBody_t b = {
            ask.mass,
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( maxSide, ask.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_ASTEROID,  currentI,  aa->uid },
                { pns::GE_STAR,      k,         ask.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    // запоминаем силу
    aa->force[ 0 ] = force[ 0 ];
    aa->force[ 1 ] = force[ 1 ];
    aa->force[ 2 ] = force[ 2 ];

    // новая скорость
    const real_t f = sqrt(
        force[ 0 ] * force[ 0 ] +
        force[ 1 ] * force[ 1 ] +
        force[ 2 ] * force[ 2 ]
    );
    if (f > 0.0) {
        const real_t inv = 1.0 / f;
        force[ 0 ] *= inv;
        force[ 1 ] *= inv;
        force[ 2 ] *= inv;
    }
    const real_t acceleration = f / aa->mass;
    const real_t v = acceleration * timestep();
    aa->velocity[ 0 ] += force[ 0 ] * v;
    aa->velocity[ 1 ] += force[ 1 ] * v;
    aa->velocity[ 2 ] += force[ 2 ] * v;

    // новые координаты
    aa->coord[ 0 ] += aa->velocity[ 0 ] * timestep();
    aa->coord[ 1 ] += aa->velocity[ 1 ] * timestep();
    aa->coord[ 2 ] += aa->velocity[ 2 ] * timestep();
}





inline void EngineCPU::planetImpactIn(
    pns::aboutPlanet_t* ap,
    size_t currentI
) {
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& observer = topology.observer;
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // рассчитаем результирующую силу
    real_t force[ 3 ] = { 0, 0, 0 };
    pns::aboutBody_t a = {
        ap->mass,
        { ap->coord[ 0 ],    ap->coord[ 1 ],    ap->coord[ 2 ] },
        { ap->rotation[ 0 ], ap->rotation[ 1 ], ap->rotation[ 2 ] }
    };


    // действие астероидов
    // # Слишком малы, чтобы гравитационно воздействовать на планету.


    // действие планет
    for (size_t k = 0; k < pns::PLANET_COUNT; ++k) {
        if (currentI == k) {
            // сама на себя не действует
            continue;
        }
        // силы гравитации, действующие на планету со стороны
        // планеты с индексом 'k'
        const pns::aboutPlanet_t& apk = planet[ k ];
        if ( pns::absentPlanet( apk ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const pns::aboutBody_t b = {
            apk.mass,
            { apk.coord[ 0 ],    apk.coord[ 1 ],    apk.coord[ 2 ] },
            { apk.rotation[ 0 ], apk.rotation[ 1 ], apk.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( ap->radius, apk.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::event_t event = {
                // uid события
                pns::E_COLLISION,
                // pi второй участник события
                { pns::GE_PLANET, k, apk.uid }
            };
            pns::planetMemorizeEvent( &ap->memoryEvent, event );
#ifdef _DEBUG
            pns::printEvent( pns::GE_PLANET, ap->uid, event, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    // действие звёзд
    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        // силы гравитации, действующие на планету со стороны
        // звезды с индексом 'k'
        const pns::aboutStar_t& ask = star[ k ];
        if ( pns::absentStar( ask ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const pns::aboutBody_t b = {
            ask.mass,
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( ap->radius, ask.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::event_t event = {
                // uid события
                pns::E_COLLISION,
                // pi второй участник события
                { pns::GE_STAR, k, ask.uid }
            };
            pns::planetMemorizeEvent( &ap->memoryEvent, event );
#ifdef _DEBUG
            pns::printEvent( pns::GE_PLANET, ap->uid, event, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    // запоминаем силу
    ap->force[ 0 ] = force[ 0 ];
    ap->force[ 1 ] = force[ 1 ];
    ap->force[ 2 ] = force[ 2 ];

    // новая скорость
    const real_t f = sqrt(
        force[ 0 ] * force[ 0 ] +
        force[ 1 ] * force[ 1 ] +
        force[ 2 ] * force[ 2 ]
    );
    if (f > 0.0) {
        const real_t inv = 1.0 / f;
        force[ 0 ] *= inv;
        force[ 1 ] *= inv;
        force[ 2 ] *= inv;
    }
    const real_t acceleration = f / ap->mass;
    const real_t v = acceleration * timestep();
    ap->velocity[ 0 ] += force[ 0 ] * v;
    ap->velocity[ 1 ] += force[ 1 ] * v;
    ap->velocity[ 2 ] += force[ 2 ] * v;

    // новые координаты
    ap->coord[ 0 ] += ap->velocity[ 0 ] * timestep();
    ap->coord[ 1 ] += ap->velocity[ 1 ] * timestep();
    ap->coord[ 2 ] += ap->velocity[ 2 ] * timestep();
    
    /* @test
    ap->test[ 1 ] = f;
    ap->test[ 2 ] = acceleration;
    ap->test[ 3 ] = v;
    */
}





inline void EngineCPU::starImpactIn(
    pns::aboutStar_t* as,
    size_t currentI
) {
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& observer = topology.observer;
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // рассчитаем результирующую силу
    real_t force[ 3 ] = { 0, 0, 0 };
    pns::aboutBody_t a = {
        as->mass,
        { as->coord[ 0 ],    as->coord[ 1 ],    as->coord[ 2 ] },
        { as->rotation[ 0 ], as->rotation[ 1 ], as->rotation[ 2 ] }
    };


    // действие астероидов
    // # Слишком малы, чтобы гравитационно воздействовать на звезду.
    // отрабатываем только события, важные для звезды
    for (size_t k = 0; k < pns::ASTEROID_COUNT; ++k) {
        // события, регистрируемые звездой со стороны астероида 'k'
        const pns::aboutAsteroid_t& aak = asteroid[ k ];
        if ( pns::absentAsteroid( aak ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const real_t collisionDistance =
            typelib::max( as->radius, aak.size[ 0 ], aak.size[ 1 ], aak.size[ 2 ] );
        const bool hasCollision =
            collision( as->coord, aak.coord, collisionDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_STAR,      currentI,  as->uid },
                { pns::GE_ASTEROID,  k,         aak.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    // действие планет
    // # Слишком малы, чтобы гравитационно воздействовать на звезду.


    // действие звёзд
    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        if (currentI == k) {
            // сама на себя не действует
            continue;
        }
        // силы гравитации, действующие на звезду со стороны
        // звезды с индексом 'k'
        const pns::aboutStar_t& ask = star[ k ];
        if ( pns::absentStar( ask ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        const pns::aboutBody_t b = {
            ask.mass,
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( as->radius, ask.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::event_t event = {
                // uid события
                pns::E_COLLISION,
                // pi второй участник события
                { pns::GE_STAR, k, ask.uid }
            };
            pns::starMemorizeEvent( &as->memoryEvent, event );
#ifdef _DEBUG
            pns::printEvent( pns::GE_STAR, as->uid, event, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    // запоминаем силу
    as->force[ 0 ] = force[ 0 ];
    as->force[ 1 ] = force[ 1 ];
    as->force[ 2 ] = force[ 2 ];

    // новая скорость
    const real_t f = sqrt(
        force[ 0 ] * force[ 0 ] +
        force[ 1 ] * force[ 1 ] +
        force[ 2 ] * force[ 2 ]
    );
    if (f > 0.0) {
        const real_t inv = 1.0 / f;
        force[ 0 ] *= inv;
        force[ 1 ] *= inv;
        force[ 2 ] *= inv;
    }
    const real_t acceleration = f / as->mass;
    const real_t v = acceleration * timestep();
    as->velocity[ 0 ] += force[ 0 ] * v;
    as->velocity[ 1 ] += force[ 1 ] * v;
    as->velocity[ 2 ] += force[ 2 ] * v;

    // новые координаты
    as->coord[ 0 ] += as->velocity[ 0 ] * timestep();
    as->coord[ 1 ] += as->velocity[ 1 ] * timestep();
    as->coord[ 2 ] += as->velocity[ 2 ] * timestep();
}




inline void EngineCPU::dealEvent() {
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& observer = topology.observer;

    // просмотрим память наблюдателя и передадим события в память участников

    // просмотр начинаем с последнего запомненного, в обратном порядке,
    // до тех пор, пока не просмотрим всё
    assert( (observer.memoryEventTwo.waldo < pns::OBSERVER_EVENT_TWO_COUNT)
        && "Валдо находится за пределами выделенной для событий памяти." );
    for (int k = observer.memoryEventTwo.waldo - 1; k != -1; --k) {
        pns::eventTwo_t& eventTwo = observer.memoryEventTwo.content[ k ];
        switch ( eventTwo.uid ) {
            case pns::E_NONE:
                // нам не интересны пустые события
                break;

            case pns::E_COLLISION:
                dealEventCollision( &eventTwo );
                break;

            default:
                assert( false
                    && "Неизвестное событие." );
        }

    } // for (int k = ...


    // из-за того, что наблюдатель раздаёт события *всем* участникам (см.
    // выше), события могут дублироваться, корректируем
    uniqueEvent();
}




inline void EngineCPU::uniqueEvent() {
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // просмотрим память элементов и удалим дубликаты событий
    // # Кандидат на параллельную обработку. Вынесено в utils.h.

    for (size_t k = 0; k < pns::ASTEROID_COUNT; ++k) {
        pns::aboutAsteroid_t& e = asteroid[ k ];
        if ( pns::absentAsteroid( e ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        pns::asteroidUniqueEvent( &e.memoryEvent );
    }


    for (size_t k = 0; k < pns::PLANET_COUNT; ++k) {
        pns::aboutPlanet_t& e = planet[ k ];
        if ( pns::absentPlanet( e ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        pns::planetUniqueEvent( &e.memoryEvent );
    }


    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        pns::aboutStar_t& e = star[ k ];
        if ( pns::absentStar( e ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        pns::starUniqueEvent( &e.memoryEvent );
    }
}




inline void EngineCPU::dealEventCollision( pns::eventTwo_t* eventTwo ) {

    auto& topology = mPortulan.lock()->topology().topology();
    auto& observer = topology.observer;
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // @todo fine? Переписать обработку событий через классы.
    const auto geA = eventTwo->piA.ge;
    const auto geB = eventTwo->piB.ge;
    const auto iA = eventTwo->piA.ii;
    const auto iB = eventTwo->piB.ii;
    switch ( geA ) {
        case pns::GE_ASTEROID:
            switch ( geB ) {
                case pns::GE_STAR:
                    dealEventCollision( eventTwo,  &asteroid[ iA ],  &star[ iB ] );
                    break;
            }
            break;

        case pns::GE_STAR:
            switch ( geB ) {
                case pns::GE_ASTEROID:
                    dealEventCollision( eventTwo,  &star[ iA ],  &asteroid[ iB ] );
                    break;
            }
            break;

        // @todo ...

    } // switch ( geA )
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*       eventTwo,
    pns::aboutAsteroid_t*  aa,
    pns::aboutStar_t*      as
) {
    // разбиваем событие на части и передаём участникам

    // I Астероид столкнулся со звездой
    const pns::event_t eventAsteroid = {
        // uid события
        pns::E_COLLISION,
        // другой участник события
        eventTwo->piB
    };
    pns::asteroidMemorizeEvent( &aa->memoryEvent, eventAsteroid );

    // II Звезда столкнулась с астероидом
    const pns::event_t eventStar = {
        // uid события
        pns::E_COLLISION,
        // другой участник события
        eventTwo->piA
    };
    pns::starMemorizeEvent( &as->memoryEvent, eventStar );

    // # Отработанное событие наблюдатель забывает.
    forgetEventTwo( eventTwo );
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*       eventTwo,
    pns::aboutStar_t*      as,
    pns::aboutAsteroid_t*  aa
) {
    // разбиваем событие на части и передаём участникам

    // I Звезда столкнулась с астероидом
    const pns::event_t eventStar = {
        // uid события
        pns::E_COLLISION,
        // другой участник события
        eventTwo->piB
    };
    pns::starMemorizeEvent( &as->memoryEvent, eventStar );

    // II Астероид столкнулся со звездой
    const pns::event_t eventAsteroid = {
        // uid события
        pns::E_COLLISION,
        // другой участник события
        eventTwo->piA
    };
    pns::asteroidMemorizeEvent( &aa->memoryEvent, eventAsteroid );

    // # Отработанное событие наблюдатель забывает.
    forgetEventTwo( eventTwo );
}




inline bool EngineCPU::forceGravityBodyAImpactIn(
    real_t force[ 3 ],
    const pns::aboutBody_t& a,
    const pns::aboutBody_t& b,
    real_t noForceDistance
) {
    // расстояние
    real_t r[ 3 ] = {
        b.coord[ 0 ] - a.coord[ 0 ],
        b.coord[ 1 ] - a.coord[ 1 ],
        b.coord[ 2 ] - a.coord[ 2 ],
    };
    const real_t distance2 = (
        r[ 0 ] * r[ 0 ] +
        r[ 1 ] * r[ 1 ] +
        r[ 2 ] * r[ 2 ]
    );
    if (distance2 < (noForceDistance * noForceDistance)) {
        // при столкновениях отключаем силу
        return false;
    }
    const real_t distance = sqrt( distance2 );
    if (distance > 0.0) {
        const real_t inv = 1.0 / distance;
        r[ 0 ] *= inv;
        r[ 1 ] *= inv;
        r[ 2 ] *= inv;
    }

    // сила гравитации
    const real_t f =
        typelib::constant::physics::G * a.mass /
        distance2 * b.mass;

    force[ 0 ] += r[ 0 ] * f;
    force[ 1 ] += r[ 1 ] * f;
    force[ 2 ] += r[ 2 ] * f;

    return true;
}





inline bool EngineCPU::collision(
    const real_t coordA[ 3 ],
    const real_t coordB[ 3 ],
    real_t collisionDistance
) {
    // расстояние
    const real_t r[ 3 ] = {
        coordA[ 0 ] - coordB[ 0 ],
        coordA[ 1 ] - coordB[ 1 ],
        coordA[ 2 ] - coordB[ 2 ],
    };
    const real_t distance2 = (
        r[ 0 ] * r[ 0 ] +
        r[ 1 ] * r[ 1 ] +
        r[ 2 ] * r[ 2 ]
    );

    return (distance2 < (collisionDistance * collisionDistance));
}





inline void EngineCPU::notifyAndCompleteEvent() {
    // каждый элемент звёздной системы хранит информацию о событиях,
    // которые с ним произошли

    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // # Запоминаем некоторые характеристики, изменений которых
    //   ждут подписчики.
    pns::deltaElement_t delta = {};

    // события у астероидов
    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        auto& e = asteroid[ i ];
        if ( pns::presentAsteroid( e ) ) {
            notifyAndCompleteEvent( &e, i, delta );
        }
    }

    // события у планет
    for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
        auto& e = planet[ i ];
        if ( pns::presentPlanet( e ) ) {
            notifyAndCompleteEvent( &e, i, delta );
        }
    }

    // события у звёзд
    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        auto& e = star[ i ];
        if ( pns::presentStar( e ) ) {
            notifyAndCompleteEvent( &e, i, delta );
        }
    }


    // оптимизируем списки, уведомляем подписчиков о прочих изменениях
    // #! Оптимизация должна быть только после всех уведомлений / отработок.
    //    Иначе можем получить искажённую картину, т.к. порядок отработки
    //    событий не определён.
    // @see #Соглашения в начале метода.
    if (delta.asteroid.count != 0) {
        pns::optimizeCountAsteroid( asteroid );
        const size_t n = pns::countAsteroid( asteroid, true );
        afterChangeCountAsteroid( n, delta.asteroid.count );
    }

    if (delta.planet.count != 0) {
        pns::optimizeCountPlanet( planet );
        const size_t n = pns::countPlanet( planet, true );
        afterChangeCountPlanet( n, delta.planet.count );
    }

    if (delta.star.count != 0) {
        pns::optimizeCountStar( star );
        const size_t n = pns::countStar( star, true );
        afterChangeCountStar( n, delta.star.count );
    }
}




inline void EngineCPU::notifyAndCompleteEvent(
    pns::aboutAsteroid_t*  aa,
    size_t                 currentI,
    pns::deltaElement_t&   delta
) {
    assert( !mPortulan.expired() );

    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // просматриваем события, отрабатываем, забываем

    // просмотр начинаем с последнего запомненного, в обратном порядке,
    // до тех пор, пока не просмотрим всё
    assert( (aa->memoryEvent.waldo < pns::ASTEROID_EVENT_COUNT)
        && "Валдо находится за пределами выделенной для событий памяти." );
    for (int k = aa->memoryEvent.waldo - 1; k != aa->memoryEvent.waldo; --k) {
        if (k < 0) {
            k = pns::ASTEROID_EVENT_COUNT - 1;
        }
        pns::event_t& event = aa->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // нам не интересны пустые события
            continue;
        }

        // астероид столкнулся с другим элементом звёздной системы
        if (event.uid == pns::E_COLLISION) {

            // астероид столкнулся со звездой
            if (event.pi.ge == pns::GE_STAR) {
                notifyAndCompleteEventAsteroidCollisionStar(
                    asteroid,  currentI,
                    star,      event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // астероид столкнулся с планетой
            if (event.pi.ge == pns::GE_PLANET) {
                notifyAndCompleteEventAsteroidCollisionPlanet(
                    asteroid,  currentI,
                    planet,    event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // астероид столкнулся с другим астероидом
            if (event.pi.ge == pns::GE_ASTEROID) {
                notifyAndCompleteEventAsteroidCollisionAsteroid(
                    asteroid,  currentI,
                    asteroid,  event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)

        // @todo ...
    }


    // все события отработаны, cбрасываем валдо
    aa->memoryEvent.waldo = 0;
}




inline void EngineCPU::notifyAndCompleteEvent(
    pns::aboutPlanet_t*   ap,
    size_t                currentI,
    pns::deltaElement_t&  delta
) {
    assert( !mPortulan.expired() );

    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // просматриваем события, отрабатываем, забываем

    // просмотр начинаем с последнего запомненного, в обратном порядке,
    // до тех пор, пока не просмотрим всё
    assert( (ap->memoryEvent.waldo < pns::PLANET_EVENT_COUNT)
        && "Валдо находится за пределами выделенной для событий памяти." );
    for (int k = ap->memoryEvent.waldo - 1; k != ap->memoryEvent.waldo; --k) {
        if (k < 0) {
            k = pns::PLANET_EVENT_COUNT - 1;
        }
        pns::event_t& event = ap->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // нам не интересны пустые события
            continue;
        }

        // планета столкнулась с другим элементом звёздной системы
        if (event.uid == pns::E_COLLISION) {

            // планета столкнулась со звездой
            if (event.pi.ge == pns::GE_STAR) {
                notifyAndCompleteEventPlanetCollisionStar(
                    planet,  currentI,
                    star,    event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // планета столкнулась с другой планетой
            if (event.pi.ge == pns::GE_PLANET) {
                notifyAndCompleteEventPlanetCollisionPlanet(
                    planet,  currentI,
                    planet,  event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // планета столкнулась с астероидом
            if (event.pi.ge == pns::GE_ASTEROID) {
                notifyAndCompleteEventPlanetCollisionAsteroid(
                    planet,    currentI,
                    asteroid,  event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)

        // @todo ...
    }


    // все события отработаны, cбрасываем валдо
    ap->memoryEvent.waldo = 0;
}




inline void EngineCPU::notifyAndCompleteEvent(
    pns::aboutStar_t*     as,
    size_t                currentI,
    pns::deltaElement_t&  delta
) {
    assert( !mPortulan.expired() );

    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // просматриваем события, отрабатываем, забываем

    // просмотр начинаем с последнего запомненного, в обратном порядке,
    // до тех пор, пока не просмотрим всё
    assert( (as->memoryEvent.waldo < pns::STAR_EVENT_COUNT)
        && "Валдо находится за пределами выделенной для событий памяти." );
    for (int k = as->memoryEvent.waldo - 1; k != as->memoryEvent.waldo; --k) {
        if (k < 0) {
            k = pns::STAR_EVENT_COUNT - 1;
        }
        pns::event_t& event = as->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // нам не интересны пустые события
            continue;
        }

        // звезда столкнулась с другим элементом звёздной системы
        if (event.uid == pns::E_COLLISION) {

            // звезда столкнулась с другой звездой
            if (event.pi.ge == pns::GE_STAR) {
                notifyAndCompleteEventStarCollisionStar(
                    star,  currentI,
                    star,  event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // звезда столкнулась с планетой
            if (event.pi.ge == pns::GE_PLANET) {
                notifyAndCompleteEventStarCollisionPlanet(
                    star,    currentI,
                    planet,  event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // звезда столкнулась с астероидом
            if (event.pi.ge == pns::GE_ASTEROID) {
                notifyAndCompleteEventStarCollisionAsteroid(
                    star,      currentI,
                    asteroid,  event.pi.ii,
                    delta
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)

        // @todo ...
    }


    // все события отработаны, cбрасываем валдо
    as->memoryEvent.waldo = 0;
}





inline void EngineCPU::statistics() {
    const auto& os = mPortulan.lock()->orderStatistics();
    for (auto itr = os.cbegin(); itr != os.cend(); ++itr) {
        itr->second->grabPulse( mLive.pulselive() );
    }
}




inline void EngineCPU::calcOptimalTimestep( int step ) {
    // вычисляем оптимальный шаг времени для системы в тек. состоянии
    const real_t eta = timeConverge();
    const real_t STEP = static_cast< real_t >( step * 2);
    timestep( eta / STEP );
    if (timestep() < 1.0) {
        timestep( 1.0 );
    }
}





inline void EngineCPU::harvestStatisticsForOptimizeTimestep() {

    assert( !mPortulan.expired() );
    const auto& topology = mPortulan.lock()->topology().topology();
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // чтобы не создавать лапшевидный код, сольём нужные данные в
    // единый список
    std::vector< pns::point_t >  point;

    const auto pushPoint = [ &point ] (
        const real_t  coord[ 3 ],
        const real_t  velocity[ 3 ]
    ) {
        const auto c = typelib::CoordT< real_t >( coord );
        const auto v = typelib::VectorT< real_t >( velocity );
        const pns::point_t p = {
            c,
            v,
            v.length()
        };
        point.push_back( p );
    };

    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        const auto body = asteroid[ i ];
        if ( pns::presentAsteroid( body ) ) {
            pushPoint( body.coord, body.velocity );
        }
    }

    for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
        const auto body = planet[ i ];
        if ( pns::presentPlanet( body ) ) {
            pushPoint( body.coord, body.velocity );
        }
    }

    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        const auto body = star[ i ];
        if ( pns::presentStar( body ) ) {
            pushPoint( body.coord, body.velocity );
        }
    }

    // @todo optimize Предварительно кластеризировать по координатам,
    //       скоростям. Вести кластера в процессе работы движка?


    // соберём статистику

    // расстояния, скорости, время сближения
    for (auto itr = point.cbegin(); itr != point.cend(); ++itr) {
        for (auto jtr = itr; jtr != point.cend(); ++jtr) {
            if (itr != jtr) {
                const double distance = itr->coord.distance( jtr->coord );
                if (distance < mStatistics.distance[ 0 ]) {
                    mStatistics.distance[ 0 ] = distance;
                } else if (distance > mStatistics.distance[ 1 ]) {
                    mStatistics.distance[ 1 ] = distance;
                }

                if (itr->lengthVelocity < mStatistics.velocity[ 0 ]) {
                    mStatistics.velocity[ 0 ] = itr->lengthVelocity;
                } else if (itr->lengthVelocity > mStatistics.velocity[ 1 ]) {
                    mStatistics.velocity[ 1 ] = itr->lengthVelocity;
                }

                const real_t eta = estimateTimeApproaching( *itr, *jtr );
                // тела могут удаляться друг от друга
                if (eta > 0) {
                    if (eta < mStatistics.velocity[ 0 ]) {
                        mStatistics.estimateTimeApproaching[ 0 ] = eta;
                    } else if (eta > mStatistics.velocity[ 1 ]) {
                        mStatistics.estimateTimeApproaching[ 1 ] = eta;
                    }
                }
            }
        }
    }

}




inline EngineCPU::real_t EngineCPU::estimateTimeApproaching(
    const pns::point_t& a,
    const pns::point_t& b
) {
    if ( typelib::empty( a.lengthVelocity ) && typelib::empty( b.lengthVelocity ) ) {
        // неподвижные тела никогда не сблизятся
        return std::numeric_limits< real_t >::infinity();
    }

    if (a.coord == b.coord) {
        // точки уже вместе
        return 0.0;
    }


    typedef typelib::LineT< real_t >    line_t;
    typedef typelib::CoordT< real_t >   coord_t;
    typedef typelib::VectorT< real_t >  vector_t;

    // находим минимальное расстояние между линиями скоростей
    // неподвижному телу даём вектор скорости направленный к другому телу
    const vector_t va = typelib::empty( a.lengthVelocity ) ?
        static_cast< vector_t >( b.coord - a.coord ) : a.velocity;
    const line_t  la( a.coord, va );

    const vector_t vb = typelib::empty( b.lengthVelocity ) ?
        static_cast< vector_t >( a.coord - b.coord ) : b.velocity;
    const line_t  lb( b.coord, vb );

    coord_t  pa, pb;
    const real_t distance = la.distance( lb, &pa, &pb );

    // находим время, необх. телу 'a', чтобы приблизиться к этому расстоянию
    const real_t distanceA =
        static_cast< vector_t >( a.coord - pa ).length();
    const real_t eta = distanceA / a.lengthVelocity;

    return eta;
}


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
