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
    // # Все элементы звёздной системы работают по единой схеме.
    // # Каждый элемент звёздной системы генерирует свой набор событий.
    // # Испускаемые элементом события хранятся в памяти событий у самого
    //   элемента.
    // # Конкретные события (могут быть отработаны самим элементом) и общие
    //   события (требуют для отработки уведомить второго участника) хранятся
    //   в разных линейках памяти.
    // # Чтобы избежать дублирования симметричных общих событий (например,
    //   столкновений), принимаем:
    //   симметр. общее событие регистрируется *только* первым по алфавиту
    //   элементом, остальные участники просматривают такие события и создают
    //   на их основе конкретные (свои) события. Побочный эффект: меньше
    //   загрузка памяти общих событий, ускорение обработки.
    // #? Элемент может заполнить всю память конкретных событий и только
    //   часть памяти с общими. Ограничение на заполнение памяти с общими
    //   событиями связано с тем, что память будет дозаполнена общими событиями
    //   от других элементов.
    // # Пройдя все свои возможные события *или* заполнив память
    //   событий, элемент ожидает синхронизации с другими элементами.
    // # После синхронизации


    // # Реализуем т.о., чтобы минимальными усилиями перенести на OpenCL.
    //   Для этого (1) запомним события с двумя и более участниками в
    //   памяти наблюдателя (observer_t), чтобы (2) после уведомить всех
    //   участников события, а потом (3) корректно и *независимо* отработать
    //   события каждым участником.

    // (1) запомним события с двумя и более участниками в памяти наблюдателя
    //     и, если события простые, в памяти каждого участника

    // воздействие на астероиды
    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        auto& e = asteroid[ i ];
        if ( pns::presentAsteroid( e ) ) {
            asteroidImpactIn( &e, i );
        }
    }

    // воздействие на планеты
    for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
        auto& e = planet[ i ];
        if ( pns::presentPlanet( e ) ) {
            planetImpactIn( &e, i );
        }
    }

    // воздействие на звёзды
    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        auto& e = star[ i ];
        if ( pns::presentStar( e ) ) {
            starImpactIn( &e, i );
        }
    }


    // (2) распределим события между участниками
    dealEventTwo();


    // (3) корректно и *независимо* отработаем события каждым участником,
    //     уведомим слушателей
    notifyAndCompleteEvent();


    // мир становится старше
    mLive.inc( mTimestep );

    // пульс пройден
    completePulse();


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
        pns::massAsteroid( *aa ),
        { aa->coord[ 0 ],    aa->coord[ 1 ],    aa->coord[ 2 ] },
        { aa->rotation[ 0 ], aa->rotation[ 1 ], aa->rotation[ 2 ] }
    };

    const real_t maxSide =
        typelib::max( aa->size[ 0 ], aa->size[ 1 ], aa->size[ 2 ] );


    // действие астероидов
    // # Слишком малы, чтобы гравитационно воздействовать на себя.
#if 1
    // Но вполне могут столкнуться друг с другом.
    for (size_t k = 0; k < pns::ASTEROID_COUNT; ++k) {
        if (currentI == k) {
            // сам на себя не действует
            continue;
        }
        const pns::aboutAsteroid_t& aak = asteroid[ k ];
        if ( pns::absentAsteroid( aak ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            break;
        }
        // # Столкновение фиксируем при малейшей вероятности (по размерам).
        const real_t collisionDistance =
            typelib::max( maxSide, aak.size[ 0 ], aak.size[ 1 ], aak.size[ 2 ] );
        const bool hasCollision =
            collision( aa->coord, aak.coord, collisionDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_ASTEROID,  currentI,  aa->uid },
                { pns::GE_ASTEROID,  k,         aak.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
#endif
        }

    } // for (size_t k = 0 ...
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
            pns::massPlanet( apk ),
            { apk.coord[ 0 ],    apk.coord[ 1 ],    apk.coord[ 2 ] },
            { apk.rotation[ 0 ], apk.rotation[ 1 ], apk.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( maxSide, apk.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_ASTEROID,  currentI,  aa->uid },
                { pns::GE_PLANET,    k,         apk.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
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
            pns::massStar( ask ),
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


    /* - Переписано ч/з события. См. ниже.
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
    const real_t acceleration = f / pns::massAsteroid( *aa );
    const real_t v = acceleration * timestep();
    aa->velocity[ 0 ] += force[ 0 ] * v;
    aa->velocity[ 1 ] += force[ 1 ] * v;
    aa->velocity[ 2 ] += force[ 2 ] * v;

    // новые координаты
    aa->coord[ 0 ] += aa->velocity[ 0 ] * timestep();
    aa->coord[ 1 ] += aa->velocity[ 1 ] * timestep();
    aa->coord[ 2 ] += aa->velocity[ 2 ] * timestep();
    */

    // воздействие силы
    const auto absForce = sqrt(
        force[ 0 ] * force[ 0 ] +
        force[ 1 ] * force[ 1 ] +
        force[ 2 ] * force[ 2 ]
    );
    pns::real_t nf[ 3 ] = { force[ 0 ],  force[ 1 ],  force[ 2 ] };
    if (absForce > 0.0) {
        const auto inv = 1.0 / absForce;
        nf[ 0 ] *= inv;  nf[ 1 ] *= inv;  nf[ 2 ] *= inv;
    }
    const auto absAcceleration = absForce / pns::massAsteroid( *aa );
    const auto absDeltaVelocity = absAcceleration * timestep();
    const pns::real_t velocity[ 3 ] = {
        nf[ 0 ] * absDeltaVelocity,
        nf[ 1 ] * absDeltaVelocity,
        nf[ 2 ] * absDeltaVelocity
    };
    if (absForce >= MIN_IMPACT_FORCE_PORTE) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_IMPACT_FORCE,
            // pi второй участник события - здесь не важен
            {},
            // характеристика
            { force[ 0 ],  force[ 1 ],  force[ 2 ],  absForce }
        };
        pns::asteroidMemorizeEvent( &aa->memoryEvent, event );

        // под действием силы возникают и другие события
        // изменение скорости
        if (absDeltaVelocity >= MIN_CHANGE_VELOCITY_PORTE) {
            const pns::eventTwo_t event = {
                // uid события
                pns::E_CHANGE_VELOCITY,
                // pi второй участник события - здесь не важен
                {},
                // характеристика
                { velocity[ 0 ],  velocity[ 1 ],  velocity[ 2 ],  absDeltaVelocity }
            };
            pns::asteroidMemorizeEvent( &aa->memoryEvent, event );
        }

    } // if (absForce >= MIN_IMPACT_FORCE)


    // изменение координат
    // тело уже может обладать скоростью
    // # 0.01 - изменение на 1 см.
    // координаты меняет скорость
    const pns::real_t coord[ 3 ] = {
        (aa->velocity[ 0 ] + velocity[ 0 ]) * timestep(),
        (aa->velocity[ 1 ] + velocity[ 1 ]) * timestep(),
        (aa->velocity[ 2 ] + velocity[ 2 ]) * timestep()
    };
    const auto absDeltaCoord = sqrt(
        coord[ 0 ] * coord[ 0 ] +
        coord[ 1 ] * coord[ 1 ] +
        coord[ 2 ] * coord[ 2 ]
    );
    if (absDeltaCoord >= MIN_CHANGE_DISTANCE_PORTE) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_COORD,
            // pi второй участник события - здесь не важен
            {},
            // характеристика
            { coord[ 0 ],  coord[ 1 ],  coord[ 2 ],  absDeltaCoord }
        };
        pns::asteroidMemorizeEvent( &aa->memoryEvent, event );
    }

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
        pns::massPlanet( *ap ),
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
            pns::massPlanet( apk ),
            { apk.coord[ 0 ],    apk.coord[ 1 ],    apk.coord[ 2 ] },
            { apk.rotation[ 0 ], apk.rotation[ 1 ], apk.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( ap->radius, apk.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_PLANET,  currentI,  ap->uid },
                { pns::GE_PLANET,  k,         apk.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
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
            pns::massStar( ask ),
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( ap->radius, ask.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_PLANET,  currentI,  ap->uid },
                { pns::GE_STAR,    k,         ask.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    /* - Переписано ч/з события. См. ниже.
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
    const real_t acceleration = f / pns::massPlanet( *ap );
    const real_t v = acceleration * timestep();
    ap->velocity[ 0 ] += force[ 0 ] * v;
    ap->velocity[ 1 ] += force[ 1 ] * v;
    ap->velocity[ 2 ] += force[ 2 ] * v;

    // новые координаты
    ap->coord[ 0 ] += ap->velocity[ 0 ] * timestep();
    ap->coord[ 1 ] += ap->velocity[ 1 ] * timestep();
    ap->coord[ 2 ] += ap->velocity[ 2 ] * timestep();
    */
    
    /* @test
    ap->test[ 1 ] = f;
    ap->test[ 2 ] = acceleration;
    ap->test[ 3 ] = v;
    */

    // воздействие силы
    const auto absForce = sqrt(
        force[ 0 ] * force[ 0 ] +
        force[ 1 ] * force[ 1 ] +
        force[ 2 ] * force[ 2 ]
    );
    pns::real_t nf[ 3 ] = { force[ 0 ],  force[ 1 ],  force[ 2 ] };
    if (absForce > 0.0) {
        const auto inv = 1.0 / absForce;
        nf[ 0 ] *= inv;  nf[ 1 ] *= inv;  nf[ 2 ] *= inv;
    }
    const auto absAcceleration = absForce / pns::massPlanet( *ap );
    const auto absDeltaVelocity = absAcceleration * timestep();
    const pns::real_t velocity[ 3 ] = {
        nf[ 0 ] * absDeltaVelocity,
        nf[ 1 ] * absDeltaVelocity,
        nf[ 2 ] * absDeltaVelocity
    };
    if (absForce >= MIN_IMPACT_FORCE_PORTE) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_IMPACT_FORCE,
            // pi второй участник события - здесь не важен
            {},
            // характеристика
            { force[ 0 ],  force[ 1 ],  force[ 2 ],  absForce }
        };
        pns::planetMemorizeEvent( &ap->memoryEvent, event );

        // под действием силы возникают и другие события
        // изменение скорости
        if (absDeltaVelocity >= MIN_CHANGE_VELOCITY_PORTE) {
            const pns::eventTwo_t event = {
                // uid события
                pns::E_CHANGE_VELOCITY,
                // pi второй участник события - здесь не важен
                {},
                // характеристика
                { velocity[ 0 ],  velocity[ 1 ],  velocity[ 2 ],  absDeltaVelocity }
            };
            pns::planetMemorizeEvent( &ap->memoryEvent, event );
        }

    } // if (absForce >= MIN_IMPACT_FORCE)


    // изменение координат
    // тело уже может обладать скоростью
    // # 0.01 - изменение на 1 см.
    // координаты меняет скорость
    const pns::real_t coord[ 3 ] = {
        (ap->velocity[ 0 ] + velocity[ 0 ]) * timestep(),
        (ap->velocity[ 1 ] + velocity[ 1 ]) * timestep(),
        (ap->velocity[ 2 ] + velocity[ 2 ]) * timestep()
    };
    const auto absDeltaCoord = sqrt(
        coord[ 0 ] * coord[ 0 ] +
        coord[ 1 ] * coord[ 1 ] +
        coord[ 2 ] * coord[ 2 ]
    );
    if (absDeltaCoord >= MIN_CHANGE_DISTANCE_PORTE) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_COORD,
            // pi второй участник события - здесь не важен
            {},
            // характеристика
            { coord[ 0 ],  coord[ 1 ],  coord[ 2 ],  absDeltaCoord }
        };
        pns::planetMemorizeEvent( &ap->memoryEvent, event );
    }

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
        pns::massStar( *as ),
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
            pns::massStar( ask ),
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( as->radius, ask.radius );
        const bool hasCollision =
            !forceGravityBodyAImpactIn( force,  a, b, noForceDistance );
        if ( hasCollision ) {
            const pns::eventTwo_t eventTwo = {
                // uid события
                pns::E_COLLISION,
                // участники события
                { pns::GE_STAR,  currentI,  as->uid },
                { pns::GE_STAR,  k,         ask.uid }
            };
            pns::observerMemorizeEventTwo( &observer.memoryEventTwo, eventTwo );
#ifdef _DEBUG
            pns::printEventTwo( eventTwo, &topology );
#endif
        }

    } // for (size_t k = 0 ...


    /* - Переписано ч/з события. См. ниже.
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
    const real_t acceleration = f / pns::massStar( *as );
    const real_t v = acceleration * timestep();
    as->velocity[ 0 ] += force[ 0 ] * v;
    as->velocity[ 1 ] += force[ 1 ] * v;
    as->velocity[ 2 ] += force[ 2 ] * v;

    // новые координаты
    as->coord[ 0 ] += as->velocity[ 0 ] * timestep();
    as->coord[ 1 ] += as->velocity[ 1 ] * timestep();
    as->coord[ 2 ] += as->velocity[ 2 ] * timestep();
    */

    // воздействие силы
    const auto absForce = sqrt(
        force[ 0 ] * force[ 0 ] +
        force[ 1 ] * force[ 1 ] +
        force[ 2 ] * force[ 2 ]
    );
    pns::real_t nf[ 3 ] = { force[ 0 ],  force[ 1 ],  force[ 2 ] };
    if (absForce > 0.0) {
        const auto inv = 1.0 / absForce;
        nf[ 0 ] *= inv;  nf[ 1 ] *= inv;  nf[ 2 ] *= inv;
    }
    const auto absAcceleration = absForce / pns::massStar( *as );
    const auto absDeltaVelocity = absAcceleration * timestep();
    const pns::real_t velocity[ 3 ] = {
        nf[ 0 ] * absDeltaVelocity,
        nf[ 1 ] * absDeltaVelocity,
        nf[ 2 ] * absDeltaVelocity
    };
    if (absForce >= MIN_IMPACT_FORCE_PORTE) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_IMPACT_FORCE,
            // pi второй участник события - здесь не важен
            {},
            // характеристика
            { force[ 0 ],  force[ 1 ],  force[ 2 ],  absForce }
        };
        pns::starMemorizeEvent( &as->memoryEvent, event );

        // под действием силы возникают и другие события
        // изменение скорости
        if (absDeltaVelocity >= MIN_CHANGE_VELOCITY_PORTE) {
            const pns::eventTwo_t event = {
                // uid события
                pns::E_CHANGE_VELOCITY,
                // pi второй участник события - здесь не важен
                {},
                // характеристика
                { velocity[ 0 ],  velocity[ 1 ],  velocity[ 2 ],  absDeltaVelocity }
            };
            pns::starMemorizeEvent( &as->memoryEvent, event );
        }

    } // if (absForce >= MIN_IMPACT_FORCE)


    // изменение координат
    // тело уже может обладать скоростью
    // # 0.01 - изменение на 1 см.
    // координаты меняет скорость
    const pns::real_t coord[ 3 ] = {
        (as->velocity[ 0 ] + velocity[ 0 ]) * timestep(),
        (as->velocity[ 1 ] + velocity[ 1 ]) * timestep(),
        (as->velocity[ 2 ] + velocity[ 2 ]) * timestep()
    };
    const auto absDeltaCoord = sqrt(
        coord[ 0 ] * coord[ 0 ] +
        coord[ 1 ] * coord[ 1 ] +
        coord[ 2 ] * coord[ 2 ]
    );
    if (absDeltaCoord >= MIN_CHANGE_DISTANCE_PORTE) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_COORD,
            // pi второй участник события - здесь не важен
            {},
            // характеристика
            { coord[ 0 ],  coord[ 1 ],  coord[ 2 ],  absDeltaCoord }
        };
        pns::starMemorizeEvent( &as->memoryEvent, event );
    }

}




inline void EngineCPU::dealEventTwo() {

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
    const auto iA  = eventTwo->piA.ii;
    const auto iB  = eventTwo->piB.ii;
    switch ( geA ) {
        case pns::GE_ASTEROID:
            switch ( geB ) {
                case pns::GE_ASTEROID:
                    dealEventCollision( eventTwo,  &asteroid[ iA ],  &asteroid[ iB ] );
                    break;
                case pns::GE_STAR:
                    dealEventCollision( eventTwo,  &asteroid[ iA ],  &star[ iB ] );
                    break;
            }
            break;

        case pns::GE_PLANET:
            switch ( geB ) {
                case pns::GE_STAR:
                    dealEventCollision( eventTwo,  &planet[ iA ],  &star[ iB ] );
                    break;
            }
            break;

        case pns::GE_STAR:
            switch ( geB ) {
                case pns::GE_ASTEROID:
                    dealEventCollision( eventTwo,  &star[ iA ],  &asteroid[ iB ] );
                    break;
                case pns::GE_PLANET:
                    dealEventCollision( eventTwo,  &star[ iA ],  &planet[ iB ] );
                    break;
            }
            break;

        // @todo ...

    } // switch ( geA )
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*       eventTwo,
    pns::aboutAsteroid_t*  a,
    pns::aboutAsteroid_t*  b
) {
    // разбиваем событие на части и передаём участникам

    // чтобы рассчитать последствия столкновения двух астероидов, надо
    // оценить некоторые характеристики удара
    // # Что произойдёт с астероидом определим по силе столкновения.
    // # Удар - прямой.
    // # Коэффициент восстановления у астероидов одинаков.
    const typelib::VectorT< pns::real_t >  va( a->velocity );
    const typelib::VectorT< pns::real_t >  vb( b->velocity );
    static const pns::real_t COR = 0.5;
    const auto massA = pns::massAsteroid( *a );
    const auto massB = pns::massAsteroid( *b );
    const auto rv = typelib::compute::physics::speedCollision(
        massA, va, massB, vb, COR
    );

    // Астероид A
    const auto& rva = rv.first;
    const auto rval = rva.length();
    const auto val = va.length();
    const auto deltaRVAL = rval - val;

    // # Малые изменения скорости проигнорируем.
    auto deltaRVA = typelib::VectorT< pns::real_t >::ZERO();
    pns::real_t deltaTemperatureAAfter = 0.0;
    const bool changeVelocityA = (std::abs( deltaRVAL ) > 0.01);
    if ( changeVelocityA ) {
        deltaRVA = rva - va;
        const auto val2 = val * val;
        const auto rval2 = rval * rval;

        // кинет. энергия до и после столкновения
        const auto kineticABefore = massA * val2 / 2.0;
        const auto kineticAAfter  = massA * rval2 / 2.0;

        // # Каждый астероид изменит температуру настолько, насколько
        //   изменилась его кинетическая энергия.
        const auto deltaKineticA = kineticAAfter - kineticABefore;
        deltaTemperatureAAfter = std::abs(
            typelib::compute::physics::deltaTemperature(
                deltaKineticA,  massA,  a->heatCapacity
        ) );
    } // if ( changeVelocityA )


    // Астероид B
    // @see Коммент. для астероида A.
    const auto& rvb = rv.second;
    const auto rvbl = rvb.length();
    const auto vbl = vb.length();
    const auto deltaRVBL = rvbl - vbl;

    auto deltaRVB = typelib::VectorT< pns::real_t >::ZERO();
    pns::real_t deltaTemperatureBAfter = 0.0;
    const bool changeVelocityB = (std::abs( deltaRVBL ) > 0.01);
    if ( changeVelocityB ) {
        deltaRVB = rvb - vb;
        const auto vbl2 = vbl * vbl;
        const auto rvbl2 = rvbl * rvbl;

        const auto kineticBBefore = massB * vbl2 / 2.0;
        const auto kineticBAfter  = massB * rvbl2 / 2.0;

        const auto deltaKineticB = kineticBAfter - kineticBBefore;
        deltaTemperatureBAfter = std::abs(
            typelib::compute::physics::deltaTemperature(
                deltaKineticB,  massB,  b->heatCapacity
        ) );
    } // if ( changeVelocityB )


    // раздаём события участникам
    if ( changeVelocityA ) {
        dealEventCollision(
            a,  deltaRVA,  deltaRVAL,  deltaTemperatureAAfter,
            *b, deltaRVB,  deltaRVBL,  deltaTemperatureBAfter,
            eventTwo->piB
        );
    }

    if ( changeVelocityB ) {
        dealEventCollision(
            b,  deltaRVB,  deltaRVBL,  deltaTemperatureBAfter,
            *a, deltaRVA,  deltaRVAL,  deltaTemperatureAAfter,
            eventTwo->piA
        );
    }


    // # Отработанное событие наблюдатель забывает.
    forgetEventTwo( eventTwo );
}




inline void EngineCPU::dealEventCollision(
    pns::aboutAsteroid_t*  a,
    const typelib::VectorT< pns::real_t >&  deltaVA,  
    pns::real_t  deltaVAL,
    pns::real_t  deltaTemperatureAAfter,

    const pns::aboutAsteroid_t&  b,
    const typelib::VectorT< pns::real_t >&  deltaVB,
    pns::real_t  deltaVBL,
    pns::real_t  deltaTemperatureBAfter,
    const pns::pointerElement_t&  pb
) {
    // Астероид A столкнулся с астероидом B
    // # Изменения касаются только элемента A.
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_COLLISION,
            // другой участник события
            pb
        };
        pns::asteroidMemorizeEvent( &a->memoryEvent, event );
    }

#ifdef _DEBUG
    std::cout << "Температура a" << a->uid << " изменилась на " <<
        deltaTemperatureAAfter << " C" <<
    std::endl;
#endif


    // помимо общего события, регистрируем последствия, которые необходимо
    // отработать в рамках этого же пульса

    // рассмотрим разницу температур
    if (
        // температура может подняться до температуры кипения...
        (deltaTemperatureAAfter >= a->boilingPoint)
        // ...или до температура плавления...
     || (deltaTemperatureAAfter >= a->meltingPoint)
    ) {
        // астероид раскалывается на расплавленные осколки
        {
            static const size_t SEED = 12345;
            static typelib::Random< size_t >  genN( 5, 10, SEED );
            const size_t n = genN.next();
            const pns::eventTwo_t event = {
                // uid события
                pns::E_CRUSH_N,
                // другой участник события - не важен для этого события
                {},
                // характеристика
                {
                    n,
                    deltaVA.x, deltaVA.y, deltaVA.z,
                    deltaTemperatureAAfter
                }
            };
            pns::asteroidMemorizeEvent( &a->memoryEvent, event );
        }
        return;
    }


    // ...или удар просто может быть очень сильным...
    const bool powerfullCollision = (deltaTemperatureAAfter >= 5.0);
    if ( powerfullCollision ) {
        // астероид раскалывается
        {
            static const size_t SEED = 12345;
            static typelib::Random< size_t >  genN( 2, 5, SEED );
            const size_t n = genN.next();
            const pns::eventTwo_t event = {
                // uid события
                pns::E_CRUSH_N,
                // другой участник события - не важен для этого события
                {},
                // характеристика
                {
                    n,
                    deltaVA.x, deltaVA.y, deltaVA.z,
                    deltaTemperatureAAfter
                }
            };
            pns::asteroidMemorizeEvent( &a->memoryEvent, event );
        }
        return;
    }


    // ...или сила удара недостаточна, чтобы расколоть астероид
    // меняется скорость астероида
    // # Проверку, что скорость изменилась достаточно, чтобы зафиксировать
    //   событие E_CHANGE_VELOCITY, сделали до вызова этого метода.
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_VELOCITY,
            // другой участник события - не важен для этого события
            {},
            // характеристика
            { deltaVA.x, deltaVA.y, deltaVA.z }
        };
        pns::asteroidMemorizeEvent( &a->memoryEvent, event );
    }
    // меняется температура астероида
    if (deltaTemperatureAAfter >= 1.0) {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_TEMPERATURE,
            // другой участник события - не важен для этого события
            {},
            // характеристика
            { deltaTemperatureAAfter }
        };
        pns::asteroidMemorizeEvent( &a->memoryEvent, event );
    }
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*       eventTwo,
    pns::aboutAsteroid_t*  aa,
    pns::aboutStar_t*      as
) {
    // разбиваем событие на части и передаём участникам

    // I Астероид столкнулся со звездой
    const pns::eventTwo_t eventAsteroid = {
        // uid события
        pns::E_COLLISION,
        // другой участник события
        eventTwo->piB
    };
    pns::asteroidMemorizeEvent( &aa->memoryEvent, eventAsteroid );

    // II Звезда столкнулась с астероидом
    const pns::eventTwo_t eventStar = {
        // uid события
        pns::E_COLLISION,
        // другой участник события
        eventTwo->piA
    };
    pns::starMemorizeEvent( &as->memoryEvent, eventStar );

    // III Астероид уничтожен
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_DESTROY,
            // другой участник события - не важен для этого события
            {}
        };
        pns::asteroidMemorizeEvent( &aa->memoryEvent, event );
    }

    // IV Звезда увеличила свою массу
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_MASS,
            // другой участник события - не важен для этого события
            {},
            // характеристика
            { aa->mass.base, aa->mass.knoll }
        };
        pns::starMemorizeEvent( &as->memoryEvent, event );
    }

    // # Отработанное событие наблюдатель забывает.
    forgetEventTwo( eventTwo );
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*     eventTwo,
    pns::aboutPlanet_t*  ap,
    pns::aboutStar_t*    as
) {
    // разбиваем событие на части и передаём участникам

    // I Планета столкнулась со звездой
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_COLLISION,
            // другой участник события
            eventTwo->piB
        };
        pns::planetMemorizeEvent( &ap->memoryEvent, event );
    }

    // II Звезда столкнулась с планетой
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_COLLISION,
            // другой участник события
            eventTwo->piA
        };
        pns::starMemorizeEvent( &as->memoryEvent, event );
    }

    // III Планета уничтожена
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_DESTROY,
            // другой участник события - не важен для этого события
            {}
        };
        pns::planetMemorizeEvent( &ap->memoryEvent, event );
    }

    // IV Звезда увеличила свою массу
    {
        const pns::eventTwo_t event = {
            // uid события
            pns::E_CHANGE_MASS,
            // другой участник события - не важен для этого события
            {},
            // характеристика
            { ap->mass.base, ap->mass.knoll }
        };
        pns::starMemorizeEvent( &as->memoryEvent, event );
    }

    // # Отработанное событие наблюдатель забывает.
    forgetEventTwo( eventTwo );
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*       eventTwo,
    pns::aboutStar_t*      as,
    pns::aboutAsteroid_t*  aa
) {
    // разбиваем событие на части и передаём участникам

    // воспользуемся тем, что событие - симметрично
    pns::eventTwo_t et = *eventTwo;
    std::swap( et.piA, et.piB );
    dealEventCollision( &et, aa, as );

    // # Отработанное событие наблюдатель забывает.
    forgetEventTwo( eventTwo );
}




inline void EngineCPU::dealEventCollision(
    pns::eventTwo_t*     eventTwo,
    pns::aboutStar_t*    as,
    pns::aboutPlanet_t*  ap
) {
    // разбиваем событие на части и передаём участникам

    // воспользуемся тем, что событие - симметрично
    pns::eventTwo_t et = *eventTwo;
    std::swap( et.piA, et.piB );
    dealEventCollision( &et, ap, as );

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


    // уведомляем слушателей до отработки событий
    {
        // @todo ...
    }


    // отрабатываем события...
    // # Каждый элемент способен отработать свои события независимо,
    //   не обращаясь за инфо к другим элементам и тем более не меняя
    //   др. элементы.
    {
        // ...астероидов
        for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
            auto& e = asteroid[ i ];
            if ( pns::presentAsteroid( e ) ) {
                completeEvent( &e, i, delta );
            }
        }

        // ...планет
        for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
            auto& e = planet[ i ];
            if ( pns::presentPlanet( e ) ) {
                completeEvent( &e, i, delta );
            }
        }

        // ..звёзд
        for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
            auto& e = star[ i ];
            if ( pns::presentStar( e ) ) {
                completeEvent( &e, i, delta );
            }
        }
    }


    // уведомляем слушателей после отработки событий
    // # Отработка событий выше может пометить элемент как "не живой". Поэтому
    //   просматриваем все элементы, руководствуясь только содержимым их
    //   памяти.
    Сделать признак "Событие отработано"?
    Как быстрее?
    {
        for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
            auto& e = asteroid[ i ];
            notifyAfterCompleteEvent( e );
        }

        // ...планет
        for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
            auto& e = planet[ i ];
            notifyAfterCompleteEvent( e );
        }

        // ..звёзд
        for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
            auto& e = star[ i ];
            notifyAfterCompleteEvent( e );
        }
    }


    // оптимизируем структуры, уведомляем о прочих изменениях
    // #! Оптимизация должна быть после всех уведомлений / отработок.
    //    Иначе можем получить искажённую картину, т.к. порядок отработки
    //    событий не определён.
    // @see #Соглашения в начале метода.
    {
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
}




inline void EngineCPU::completeEvent(
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
        pns::eventTwo_t& event = aa->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // нам не интересны пустые события
            continue;
        }


        // воздействие силы
        if (event.uid == pns::E_IMPACT_FORCE) {
            const pns::real_t force[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absForce = event.fReal[ 3 ];
            completeEventAsteroidImpactForce(
                asteroid,  currentI,
                force,  absForce
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // изменение координат
        if (event.uid == pns::E_CHANGE_COORD) {
            const pns::real_t deltaCoord[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absDeltaCoord = event.fReal[ 3 ];
            completeEventAsteroidChangeCoord(
                asteroid,  currentI,
                deltaCoord,  absDeltaCoord
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // изменение скорости
        if (event.uid == pns::E_CHANGE_VELOCITY) {
            const pns::real_t deltaVelocity[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absDeltaVelocity = event.fReal[ 3 ];
            completeEventAsteroidChangeVelocity(
                asteroid,  currentI,
                deltaVelocity,  absDeltaVelocity
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // астероид столкнулся с другим элементом звёздной системы
        if (event.uid == pns::E_COLLISION) {

            // астероид столкнулся со звездой
            if (event.pi.ge == pns::GE_STAR) {
                completeEventAsteroidCollisionStar(
                    asteroid,  currentI,
                    star,      event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // астероид столкнулся с планетой
            if (event.pi.ge == pns::GE_PLANET) {
                completeEventAsteroidCollisionPlanet(
                    asteroid,  currentI,
                    planet,    event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // астероид столкнулся с другим астероидом
            if (event.pi.ge == pns::GE_ASTEROID) {
                completeEventAsteroidCollisionAsteroid(
                    asteroid,  currentI,
                    asteroid,  event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)


        // у астероида изменилась температура
        if (event.uid == pns::E_CHANGE_TEMPERATURE) {
            const pns::real_t deltaTemperature = event.fReal[ 0 ];
            completeEventAsteroidChangeTemperature(
                asteroid,  currentI,
                deltaTemperature
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // астероид раскалывается на части
        if (event.uid == pns::E_CRUSH_N) {
            const auto n = static_cast< size_t >( event.fReal[ 0 ] );
            const pns::real_t deltaVelocity[ 3 ] = {
                event.fReal[ 1 ],  event.fReal[ 2 ],  event.fReal[ 3 ]
            };
            const pns::real_t deltaTemperature = event.fReal[ 4 ];
            completeEventAsteroidCrushN(
                asteroid,  currentI,
                delta,
                n,  deltaVelocity,  deltaTemperature
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // @todo ...


        // астероид уничтожен
        if (event.uid == pns::E_DESTROY) {
            completeEventAsteroidDestroy(
                asteroid,  currentI,
                delta
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }

    } // for (int k = aa->memoryEvent.waldo - 1; ...


    // все события отработаны, cбрасываем валдо
    aa->memoryEvent.waldo = 0;
}




inline void EngineCPU::completeEvent(
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
        pns::eventTwo_t& event = ap->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // нам не интересны пустые события
            continue;
        }


        // воздействие силы
        if (event.uid == pns::E_IMPACT_FORCE) {
            const pns::real_t force[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absForce = event.fReal[ 3 ];
            completeEventPlanetImpactForce(
                planet,  currentI,
                force,  absForce
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // изменение координат
        if (event.uid == pns::E_CHANGE_COORD) {
            const pns::real_t deltaCoord[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absDeltaCoord = event.fReal[ 3 ];
            completeEventPlanetChangeCoord(
                planet,  currentI,
                deltaCoord,  absDeltaCoord
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // изменение скорости
        if (event.uid == pns::E_CHANGE_VELOCITY) {
            const pns::real_t deltaVelocity[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absDeltaVelocity = event.fReal[ 3 ];
            completeEventPlanetChangeVelocity(
                planet,  currentI,
                deltaVelocity,  absDeltaVelocity
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // планета столкнулась с другим элементом звёздной системы
        if (event.uid == pns::E_COLLISION) {

            // планета столкнулась со звездой
            if (event.pi.ge == pns::GE_STAR) {
                completeEventPlanetCollisionStar(
                    planet,  currentI,
                    star,    event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // планета столкнулась с планетой
            if (event.pi.ge == pns::GE_PLANET) {
                completeEventPlanetCollisionPlanet(
                    planet,  currentI,
                    planet,  event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // планета столкнулась с другим астероидом
            if (event.pi.ge == pns::GE_ASTEROID) {
                completeEventPlanetCollisionAsteroid(
                    planet,    currentI,
                    asteroid,  event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)


        // @todo Добавить события по аналогии с астероидом.


        // планета уничтожена
        if (event.uid == pns::E_DESTROY) {
            completeEventPlanetDestroy(
                planet,  currentI,
                delta
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }

    } // for (int k = ap->memoryEvent.waldo - 1; ...


    // все события отработаны, cбрасываем валдо
    ap->memoryEvent.waldo = 0;
}




inline void EngineCPU::completeEvent(
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
        pns::eventTwo_t& event = as->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // нам не интересны пустые события
            continue;
        }


        // воздействие силы
        if (event.uid == pns::E_IMPACT_FORCE) {
            const pns::real_t force[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absForce = event.fReal[ 3 ];
            completeEventStarImpactForce(
                star,  currentI,
                force,  absForce
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // изменение координат
        if (event.uid == pns::E_CHANGE_COORD) {
            const pns::real_t deltaCoord[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absDeltaCoord = event.fReal[ 3 ];
            completeEventStarChangeCoord(
                star,  currentI,
                deltaCoord,  absDeltaCoord
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // изменение скорости
        if (event.uid == pns::E_CHANGE_VELOCITY) {
            const pns::real_t deltaVelocity[ 3 ] =
                { event.fReal[ 0 ],  event.fReal[ 1 ],  event.fReal[ 2 ] };
            const pns::real_t absDeltaVelocity = event.fReal[ 3 ];
            completeEventStarChangeVelocity(
                star,  currentI,
                deltaVelocity,  absDeltaVelocity
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // звезда столкнулась с другим элементом звёздной системы
        if (event.uid == pns::E_COLLISION) {

            // звезда столкнулась с другой звездой
            if (event.pi.ge == pns::GE_STAR) {
                completeEventStarCollisionStar(
                    star,  currentI,
                    star,  event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // звезда столкнулась с планетой
            if (event.pi.ge == pns::GE_PLANET) {
                completeEventStarCollisionPlanet(
                    star,    currentI,
                    planet,  event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

            // звезда столкнулась с астероидом
            if (event.pi.ge == pns::GE_ASTEROID) {
                completeEventStarCollisionAsteroid(
                    star,      currentI,
                    asteroid,  event.pi.ii
                );
                // # Отработанное событие надо забыть.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)


        // @todo Добавить события по аналогии с астероидом.


        // масса звезды изменилась
        if (event.uid == pns::E_CHANGE_MASS) {
            const pns::mass_t deltaMass =
                { event.fReal[ 0 ],  event.fReal[ 1 ] };
            completeEventStarChangeMass(
                star,  currentI,
                deltaMass
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }


        // звезда уничтожена
        if (event.uid == pns::E_DESTROY) {
            completeEventStarDestroy(
                star,  currentI,
                delta
            );
            // # Отработанное событие надо забыть.
            forgetEvent( &event );
            continue;
        }

    } // for (int k = as->memoryEvent.waldo - 1; ...


    // все события отработаны, cбрасываем валдо
    as->memoryEvent.waldo = 0;
}




inline void EngineCPU::notifyAfterCompleteEvent(
    const pns::aboutAsteroid_t&  aa
) {
    // отправляем свои события другим слушателям
    // @see #Соглашение об отправке чужих событий в starsystem::Listener.
    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterPulse(
        etr->whose
    ); } }

    for (auto etr = StoreListenerStarSystem::begin();
         etr; etr = StoreListenerStarSystem::next()
    ) { if ( etr ) { etr->listener.lock()->afterAsteroidCollisionStar(
        etr->whose,  a, ia,  b, ib
    ); } }
}




inline void EngineCPU::notifyAfterCompleteEvent(
    const pns::aboutPlanet_t&  ap
) {
}




inline void EngineCPU::notifyAfterCompleteEvent(
    const pns::aboutStar_t&  as
) {
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
                const auto distance = itr->coord.distance( jtr->coord );
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
