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


    // # ����� ������ �� ���� ���������.
    ListenerStarSystem::mEngine = this;
}





inline EngineCPU::~EngineCPU() {
}




inline EngineCPU::real_t EngineCPU::timeConverge() {
    harvestStatisticsForOptimizeTimestep();
    const real_t eta = mStatistics.estimateTimeApproaching[ 0 ];
    assert( typelib::empty( eta ) && "���������� �� �������." );

    return eta;
}





inline void EngineCPU::pulse( int n ) {
    // ��������� 'n' ������
    for (int i = 0; i < n; ++i) {
        pulse();
    }

    // ��������� �����. ��� �������, *�����������*, ��� ���-��
    // ������� ����� ��� ��
    /* - @todo ����� ���������� ������� �����. �����������.
    calcOptimalTimestep( n );
    */
}






inline void EngineCPU::pulse() {

    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // ������������ ����������� �� ���� ������� �������
    // # ��������� �.�., ����� ������������ �������� ��������� �� OpenCL.

    // ����������� �� ���������
    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        auto& e = asteroid[ i ];
        if ( pns::absentAsteroid( e ) ) {
            break;
        }
        asteroidImpactIn( &e, i );
    }

    // ����������� �� �������
    for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
        auto& e = planet[ i ];
        if ( pns::absentPlanet( e ) ) {
            break;
        }
        planetImpactIn( &e, i );
    }

    // ����������� �� �����
    for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
        auto& e = star[ i ];
        if ( pns::absentStar( e ) ) {
            break;
        }
        starImpactIn( &e, i );
    }


    mLive.inc( mTimestep );


    // ����� �������
    notifyAfterPulse();


    // ������������� ������� ������, ����������� ����������
    notify();


    // �������� ���������� ��� ��������� ���������
    statistics();
}





inline void EngineCPU::asteroidImpactIn(
    pns::aboutAsteroid_t* aa,
    size_t currentI
) {
    assert( !mPortulan.expired() );
    const auto& topology = mPortulan.lock()->topology().topology();
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // ���������� �������������� ����
    real_t force[ 3 ] = { 0, 0, 0 };
    pns::aboutBody_t a = {
        aa->mass,
        { aa->coord[ 0 ],    aa->coord[ 1 ],    aa->coord[ 2 ] },
        { aa->rotation[ 0 ], aa->rotation[ 1 ], aa->rotation[ 2 ] }
    };

    const real_t maxSide =
        std::max( std::max( aa->size[ 0 ], aa->size[ 1 ] ), aa->size[ 2 ] );


    // �������� ����������
    // # ������� ����, ����� ������������� �������������� �� ����.
#if 0
    // �� ������ ����� ����������� ���� � ������.
    for (size_t k = 0; k < pns::ASTEROID_COUNT; ++k) {
        if (currentI == k) {
            // ��� �� ���� �� ���������
            continue;
        }
        const pns::aboutAsteroid_t& aak = asteroid[ k ];
        if ( pns::absent( aak ) ) {
            // # ������������� ������� - ������ ����� ������.
            break;
        }
        const real_t maxSideK =
            std::max( std::max( aak.size[ 0 ], aak.size[ 1 ] ), aak.size[ 2 ] );
        const real_t collisionDistance = std::max( maxSide, maxSideK );
        if ( collision( aa->coord, aak.coord, collisionDistance ) ) {
            memorizeNext(
                pns::GE_ASTEROID, aa->event, &countEvent,
                pns::E_COLLISION_ASTEROID, k
            );
        }
    }
#endif


    // �������� ������
    for (size_t k = 0; k < pns::PLANET_COUNT; ++k) {
        // ���� ����������, ����������� �� �������� �� �������
        // ������� � �������� 'k'
        const pns::aboutPlanet_t& apk = planet[ k ];
        if ( pns::absentPlanet( apk ) ) {
            // # ������������� ������� - ������ ����� ������.
            break;
        }
        const pns::aboutBody_t b = {
            apk.mass,
            { apk.coord[ 0 ],    apk.coord[ 1 ],    apk.coord[ 2 ] },
            { apk.rotation[ 0 ], apk.rotation[ 1 ], apk.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( maxSide, apk.radius );
        const bool collision = !forceGravityBodyAImpactIn(
            force,  a, b, noForceDistance
        );
        if ( collision ) {
            const pns::event_t event = {
                // uid �������
                pns::E_COLLISION,
                // pi ������ �������� �������
                { pns::GE_PLANET, k, apk.uid }
            };
            pns::asteroidMemorizeEvent( &aa->memoryEvent, event );
#ifdef _DEBUG
            pns::printEvent( pns::GE_ASTEROID, currentI, event, true );
#endif
        }

    } // for (size_t k = 0; k < pns::PLANET_COUNT; ++k)


    // �������� ����
    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        // ���� ����������, ����������� �� �������� �� �������
        // ������ � �������� 'k'
        const pns::aboutStar_t& ask = star[ k ];
        if ( pns::absentStar( ask ) ) {
            // # ������������� ������� - ������ ����� ������.
            break;
        }
        const pns::aboutBody_t b = {
            ask.mass,
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( maxSide, ask.radius );
        const bool collision = !forceGravityBodyAImpactIn(
            force,  a, b, noForceDistance
        );
        if ( collision ) {
            const pns::event_t event = {
                // uid �������
                pns::E_COLLISION,
                // pi ������ �������� �������
                { pns::GE_STAR, k, ask.uid }
            };
            pns::asteroidMemorizeEvent( &aa->memoryEvent, event );
#ifdef _DEBUG
            pns::printEvent( pns::GE_ASTEROID, aa->uid, event, true );
#endif
        }

    } // for (size_t k = 0; k < pns::STAR_COUNT; ++k)


    // ���������� ����
    aa->force[ 0 ] = force[ 0 ];
    aa->force[ 1 ] = force[ 1 ];
    aa->force[ 2 ] = force[ 2 ];

    // ����� ��������
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

    // ����� ����������q
    aa->coord[ 0 ] += aa->velocity[ 0 ] * timestep();
    aa->coord[ 1 ] += aa->velocity[ 1 ] * timestep();
    aa->coord[ 2 ] += aa->velocity[ 2 ] * timestep();
}





inline void EngineCPU::planetImpactIn(
    pns::aboutPlanet_t* ap,
    size_t currentI
) {
    assert( !mPortulan.expired() );
    const auto& topology = mPortulan.lock()->topology().topology();
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // ���������� �������������� ����
    real_t force[ 3 ] = { 0, 0, 0 };
    pns::aboutBody_t a = {
        ap->mass,
        { ap->coord[ 0 ],    ap->coord[ 1 ],    ap->coord[ 2 ] },
        { ap->rotation[ 0 ], ap->rotation[ 1 ], ap->rotation[ 2 ] }
    };


    // �������� ����������
    // # ������� ����, ����� ������������� �������������� �� �������.


    // �������� ������
    for (size_t k = 0; k < pns::PLANET_COUNT; ++k) {
        if (currentI == k) {
            // ���� �� ���� �� ���������
            continue;
        }
        // ���� ����������, ����������� �� ������� �� �������
        // ������� � �������� 'k'
        const pns::aboutPlanet_t& apk = planet[ k ];
        if ( pns::absentPlanet( apk ) ) {
            // # ������������� ������� - ������ ����� ������.
            break;
        }
        const pns::aboutBody_t b = {
            apk.mass,
            { apk.coord[ 0 ],    apk.coord[ 1 ],    apk.coord[ 2 ] },
            { apk.rotation[ 0 ], apk.rotation[ 1 ], apk.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( ap->radius, apk.radius );
        const bool collision = !forceGravityBodyAImpactIn(
            force,  a, b, noForceDistance
        );

    } // for (size_t k = 0; k < pns::PLANET_COUNT; ++k)


    // �������� ����
    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        // ���� ����������, ����������� �� ������� �� �������
        // ������ � �������� 'k'
        const pns::aboutStar_t& ask = star[ k ];
        if ( pns::absentStar( ask ) ) {
            // # ������������� ������� - ������ ����� ������.
            break;
        }
        const pns::aboutBody_t b = {
            ask.mass,
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( ap->radius, ask.radius );
        const bool collision = !forceGravityBodyAImpactIn(
            force,  a, b, noForceDistance
        );

    } // for (size_t k = 0; k < pns::STAR_COUNT; ++k)


    // ���������� ����
    ap->force[ 0 ] = force[ 0 ];
    ap->force[ 1 ] = force[ 1 ];
    ap->force[ 2 ] = force[ 2 ];

    // ����� ��������
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

    // ����� ����������
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
    const auto& topology = mPortulan.lock()->topology().topology();
    const auto& asteroid = topology.asteroid.content;
    const auto& planet   = topology.planet.content;
    const auto& star     = topology.star.content;

    // ���������� �������������� ����
    real_t force[ 3 ] = { 0, 0, 0 };
    pns::aboutBody_t a = {
        as->mass,
        { as->coord[ 0 ],    as->coord[ 1 ],    as->coord[ 2 ] },
        { as->rotation[ 0 ], as->rotation[ 1 ], as->rotation[ 2 ] }
    };


    // �������� ����������
    // # ������� ����, ����� ������������� �������������� �� ������.


    // �������� ������
    // # ������� ����, ����� ������������� �������������� �� ������.


    // �������� ����
    for (size_t k = 0; k < pns::STAR_COUNT; ++k) {
        if (currentI == k) {
            // ���� �� ���� �� ���������
            continue;
        }
        // ���� ����������, ����������� �� ������ �� �������
        // ������ � �������� 'k'
        const pns::aboutStar_t& ask = star[ k ];
        if ( pns::absentStar( ask ) ) {
            // # ������������� ������� - ������ ����� ������.
            break;
        }
        const pns::aboutBody_t b = {
            ask.mass,
            { ask.coord[ 0 ],    ask.coord[ 1 ],    ask.coord[ 2 ] },
            { ask.rotation[ 0 ], ask.rotation[ 1 ], ask.rotation[ 2 ] }
        };
        const real_t noForceDistance = std::max( as->radius, ask.radius );
        const bool collision = !forceGravityBodyAImpactIn(
            force,  a, b, noForceDistance
        );

    } // for (size_t k = 0; k < pns::STAR_COUNT; ++k)


    // ���������� ����
    as->force[ 0 ] = force[ 0 ];
    as->force[ 1 ] = force[ 1 ];
    as->force[ 2 ] = force[ 2 ];

    // ����� ��������
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

    // ����� ����������
    as->coord[ 0 ] += as->velocity[ 0 ] * timestep();
    as->coord[ 1 ] += as->velocity[ 1 ] * timestep();
    as->coord[ 2 ] += as->velocity[ 2 ] * timestep();
}





inline bool EngineCPU::forceGravityBodyAImpactIn(
    real_t force[ 3 ],
    const pns::aboutBody_t& a,
    const pns::aboutBody_t& b,
    real_t noForceDistance
) {
    // ����������
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
    if (distance2 <= (noForceDistance * noForceDistance)) {
        // ��� ������������� ��������� ����
        return false;
    }
    const real_t distance = sqrt( distance2 );
    if (distance > 0.0) {
        const real_t inv = 1.0 / distance;
        r[ 0 ] *= inv;
        r[ 1 ] *= inv;
        r[ 2 ] *= inv;
    }

    // ���� ����������
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
    // ����������
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

    return (distance2 <= (collisionDistance * collisionDistance));
}





inline void EngineCPU::notify() {
    // ������ ������� ������� ������� ������ ���������� � ��������,
    // ������� � ��� ���������

    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // # ���������� ��������� ��������������, ��������� �������
    //   ���� ����������.
    pns::deltaElement_t delta = {};

    // ������� � ����������
    {
        for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
            auto& e = asteroid[ i ];
            if ( pns::absentAsteroid( e ) ) {
                break;
            }
            notify( &e, i, delta );
        }

        // ���������� ����������� � ������ ����������
        // @see #���������� � ������ ������.
        if (delta.asteroid.count != 0) {
            const size_t currentCountAsteroid =
                pns::countAsteroid( asteroid, true );
            afterChangeCountAsteroid( currentCountAsteroid, delta.asteroid.count );
        }
    }


    // ������� � ������
    {
        for (size_t i = 0; i < pns::PLANET_COUNT; ++i) {
            auto& e = planet[ i ];
            if ( pns::absentPlanet( e ) ) {
                break;
            }
            notify( &e, i );
        }
    }


    // ������� � ����
    {
        for (size_t i = 0; i < pns::STAR_COUNT; ++i) {
            auto& e = star[ i ];
            if ( pns::absentStar( e ) ) {
                break;
            }
            notify( &e, i );
        }
    }
}




inline void EngineCPU::notify(
    pns::aboutAsteroid_t* aa,
    size_t currentI,
    pns::deltaElement_t& delta
) {
    assert( !mPortulan.expired() );
    auto& topology = mPortulan.lock()->topology().topology();
    auto& asteroid = topology.asteroid.content;
    auto& planet   = topology.planet.content;
    auto& star     = topology.star.content;

    // ������������� �������, ������������, ��������
    bool needOptimizeAsteroid = false;

    // �������� �������� � ���������� ������������, � �������� �������,
    // �� ��� ���, ���� �� ���������� ��
    for (int k = aa->memoryEvent.ck - 1; k != aa->memoryEvent.ck; --k) {
        if (k < 0) {
            k = pns::ASTEROID_EVENT_COUNT - 1;
        }
        pns::event_t& event = aa->memoryEvent.content[ k ];
        if (event.uid == pns::E_NONE) {
            // ��� �� ��������� ������ �������
            continue;
        }

        // �������� ���������� � ������ ��������� ������� �������
        if (event.uid == pns::E_COLLISION) {

            // �������� ���������� �� �������
            if (event.pi.ge == pns::GE_STAR) {
                notifyAfterAsteroidCollisionStar(
                    asteroid,  currentI,
                    star,      event.pi.ii,
                    delta
                );
                // ���������� ����� ������
                needOptimizeAsteroid = true;
                // # ������������ ������� ���� ������.
                forgetEvent( &event );
                continue;
            }

        } // if (event.uid == pns::E_COLLISION)

        // @todo ...
    }


    // ������������ ������ ���������
    if ( needOptimizeAsteroid ) {
        pns::optimizeAsteroid( asteroid );
    }

}




inline void EngineCPU::notify(
    pns::aboutPlanet_t* ap,
    size_t currentI
) {
}




inline void EngineCPU::notify(
    pns::aboutStar_t* as,
    size_t currentI
) {
}





inline void EngineCPU::statistics() {
    const auto& os = mPortulan.lock()->orderStatistics();
    for (auto itr = os.cbegin(); itr != os.cend(); ++itr) {
        itr->second->grabPulse( mLive.pulselive() );
    }
}




inline void EngineCPU::calcOptimalTimestep( int step ) {
    // ��������� ����������� ��� ������� ��� ������� � ���. ���������
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

    // ����� �� ��������� ����������� ���, ������ ������ ������ �
    // ������ ������
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

    // @todo optimize �������������� ���������������� �� �����������,
    //       ���������. ����� �������� � �������� ������ ������?


    // ������ ����������

    // ����������, ��������, ����� ���������
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
                // ���� ����� ��������� ���� �� �����
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
        // ����������� ���� ������� �� ���������
        return std::numeric_limits< real_t >::infinity();
    }

    if (a.coord == b.coord) {
        // ����� ��� ������
        return 0.0;
    }


    typedef typelib::LineT< real_t >    line_t;
    typedef typelib::CoordT< real_t >   coord_t;
    typedef typelib::VectorT< real_t >  vector_t;

    // ������� ����������� ���������� ����� ������� ���������
    // ������������ ���� ��� ������ �������� ������������ � ������� ����
    const vector_t va = typelib::empty( a.lengthVelocity ) ?
        static_cast< vector_t >( b.coord - a.coord ) : a.velocity;
    const line_t  la( a.coord, va );

    const vector_t vb = typelib::empty( b.lengthVelocity ) ?
        static_cast< vector_t >( a.coord - b.coord ) : b.velocity;
    const line_t  lb( b.coord, vb );

    coord_t  pa, pb;
    const real_t distance = la.distance( lb, &pa, &pb );

    // ������� �����, �����. ���� 'a', ����� ������������ � ����� ����������
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
