// @require Библиотеки из "include".


/**
* Взаимодействие физических тел в полях гравитации (область
* звёздной системы). 
* Рассчитывает взаимодействия, результаты сохраняются во временной таблице.
*
* #i NVIDIA 8800GTS работает только с float.
*/


/**
* Действие тела B на тело A: как меняется сила на тело A.
*/
inline real4_t forceGravityBodyBodyInteraction(
    const real4_t  coordA,
    const real4_t  coordB,
    const real_t   massA,
    const real_t   massB,
    const real_t   timestep
) {
    // расстояние
    const real4_t r = coordB - coordA;
    //const real_t distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    //const real_t invertDistance = rsqrt( distanceSquared );
    //const real_t distance = length( r );
    const real_t distance = sqrt( r.x * r.x + r.y * r.y + r.z * r.z );
    if (distance < PRECISION) {
        // при столкновениях отключаем силу
        return (real4_t)( 0 );
    }

    // сила гравитации
    // #! При real_t-значениях возможно переполнение.
    const real_t f = massA / (distance * distance) * G * massB;
    //const real4_t rn = r * invertDistance;
    //const real4_t rn = normalize( r );
    const real4_t rn = (real4_t)( r / distance );

    return (real4_t)( rn.x * f,  rn.y * f,  rn.z * f,  0 );
}




/**
* Действие планеты B на планету A.
*//*
inline void planetPlanetInteraction(
    __global       aboutPlanet_t*  wa,
    __global const aboutPlanet_t*  a,
    __global const aboutPlanet_t*  b,
             const real_t           timestep
) {
    // @todo optimize Можно существенно оптимизировать. См. проект
    //       OpenCL / NBody от NVIDIA.

    // расстояние
    const real4_t r = (real4_t)(
        a->coord[ 0 ] - b->coord[ 0 ],
        a->coord[ 1 ] - b->coord[ 1 ],
        a->coord[ 2 ] - b->coord[ 2 ],
        0.0
    );
    const real_t distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    const real_t distance = rsqrt( distanceSquared );

    // сила гравитации
    const real_t forceWithoutMassA = G * b->mass / distanceSquared;
    const real_t force = forceWithoutMassA * a->mass;
    const real4_t rn = r / distance;
    wa->force[ 0 ] += rn.x * force;
    wa->force[ 1 ] += rn.y * force;
    wa->force[ 2 ] += rn.z * force;

    // скорость
    //const real_t acceleration = force / a->mass;
    const real_t acceleration = forceWithoutMassA;
    const real_t velocity = acceleration * timestep;
    wa->velocity[ 0 ] += rn.x * velocity;
    wa->velocity[ 1 ] += rn.y * velocity;
    wa->velocity[ 2 ] += rn.z * velocity;
}
*/



/**
* @return Сила гравитации, действующая на планету A
*         со стороны звезды B.
*//*
inline real_t forcePlanetStarInteraction(
    __global const aboutPlanet_t*  a,
    __global const aboutStar_t*    b,
             const real_t           timestep
) {
    // @todo optimize Можно существенно оптимизировать. См. проект
    //       OpenCL / NBody от NVIDIA.

    real4_t force = 0;

    real4_t velocity = (real4_t)( a->velocity[ 0 ], a->velocity[ 1 ], a->velocity[ 2 ], 0 );
    bodyBodyGravityInteraction(
        &force, &velocity,
        (real4_t)( a->coord[ 0 ], a->coord[ 1 ], a->coord[ 2 ], 0 ),
        (real4_t)( b->coord[ 0 ], b->coord[ 1 ], b->coord[ 2 ], 0 ),
        a->mass, b->mass,
        timestep
    );
    
    wa->force[ 0 ] += force.x;
    wa->force[ 1 ] += force.y;
    wa->force[ 2 ] += force.z;
    
    wa->velocity[ 0 ] += velocity.x;
    wa->velocity[ 1 ] += velocity.y;
    wa->velocity[ 2 ] += velocity.z;

    wa->coord[ 0 ] += velocity.x * timestep;
    wa->coord[ 1 ] += velocity.y * timestep;
    wa->coord[ 2 ] += velocity.z * timestep;

    // @test
    const real4_t r =
        (real4_t)( a->coord[ 0 ], a->coord[ 1 ], a->coord[ 2 ], 0 ) -
        (real4_t)( b->coord[ 0 ], b->coord[ 1 ], b->coord[ 2 ], 0 );
    const real_t distance = length( r );
    const real_t f = a->mass / (distance * distance) * G * b->mass;
    const real4_t rn = normalize( r );
    const real_t acceleration = f / a->mass;
    const real_t v = acceleration * timestep;
    wa->test[ 0 ] = f;
    wa->test[ 1 ] = distance;
    wa->test[ 2 ] = length( velocity );
    //wa->test[ 3 ] = velocity.y;
    //wa->test[ 4 ] = velocity.z;
}
*/




/**
* Рассчитывает взаимодействие планеты с телами звёздной системы.
*/
inline void planetInteraction(
    __global       aboutPlanet_t*      wap,
    __global const aboutPlanet_t*      p,
             const uint                i,
    __global const aboutStarSystem_t*  ass,
    __global const aboutBody_t*        ab,
             const real_t               timestep
) {
    // рассчитываем результирующую силу
    real4_t force = (real4_t)( 0 );

    for (uint k = 0; k < BODY_COUNT; ++k) {
        if (k == i) {
            continue;
        }

        // силы гравитации, действующие на планету со стороны
        // тела с индексом 'k'

        const enum GROUP_ELEMENT group = ab[ k ].group;
        const real4_t ca = (real4_t)( p->coord[ 0 ], p->coord[ 1 ], p->coord[ 2 ], 0 );
        real4_t cb;
        switch ( group ) {
            case GE_PLANET:
                //planetPlanetInteraction( wap, p, &ab[ k ].content.planet, timestep );
                break;

            case GE_STAR:
                cb = (real4_t)(
                    ab[ k ].content.star.coord[ 0 ],
                    ab[ k ].content.star.coord[ 1 ],
                    ab[ k ].content.star.coord[ 2 ],
                    0
                );
                force += forceGravityBodyBodyInteraction(
                    ca,       cb,
                    p->mass,  ab[ k ].content.star.mass,
                    timestep
                );

                // @test
                //wap->test[ 0 ] = length( cb - ca );

                break;

            default:
                // другие тела не действуют на планету
                continue;
        }

    } // for (uint k = 0; k < BODY_COUNT; ++k)


    //const real_t f = length( force );
    const real_t f = sqrt( force.x * force.x + force.y * force.y + force.z * force.z );
    //const real4_t fn = normalize( force );
    const real4_t fn = (real4_t)( force / f );

    // запоминаем силу
    wap->force[ 0 ] = force.x;
    wap->force[ 1 ] = force.y;
    wap->force[ 2 ] = force.z;

    // новая скорость
    const real_t acceleration = f / p->mass;
    const real_t v = acceleration * timestep;
    wap->velocity[ 0 ] += fn.x * v;
    wap->velocity[ 1 ] += fn.y * v;
    wap->velocity[ 2 ] += fn.z * v;

    // новые координаты
    wap->coord[ 0 ] += wap->velocity[ 0 ] * timestep;
    wap->coord[ 1 ] += wap->velocity[ 1 ] * timestep;
    wap->coord[ 2 ] += wap->velocity[ 2 ] * timestep;
    
    // @test
    wap->test[ 1 ] = f;
    wap->test[ 2 ] = acceleration;
    wap->test[ 3 ] = v;
}





// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void pulseA(
    __global const aboutStarSystem_t*  ass,      // 0
    __global const aboutBody_t*        ab,       // 1
    __global       aboutBody_t*        wab,      // 2
             const real_t              timestep  // 3
) {
    const uint i = get_global_id( 0 );

    // # Актуализируем данные в 'wb', чтобы проще было зафиксировать
    //   результат. См. fix*.cl.
    wab[ i ] = ab[ i ];

    const enum GROUP_ELEMENT group = ab[ i ].group;

    // # Поля для физ. тел. могут располагаться в любом порядке.
    // @todo bad optimize Утверждённый порядок = возможность оптимизации.
    switch ( group ) {
        case GE_PLANET:
            planetInteraction(
                &wab[ i ].content.planet,
                &ab[ i ].content.planet,
                i, ass, ab, timestep
            );
            break;

        case GE_STAR:
            // ...
            break;

        default:
            // не обрабатываем другие элементы
            break;
    }

}
