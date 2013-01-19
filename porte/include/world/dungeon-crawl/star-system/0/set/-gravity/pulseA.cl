// @require Библиотеки из "include".


/**
* Взаимодействие физических тел в полях гравитации (область
* звёздной системы). 
* Рассчитывает взаимодействия, результаты сохраняются во временной таблице.
*
* #i NVIDIA 8800GTS работает только с float.
*/


inline void forceGravityBodyBodyInteraction(
    real_t* fx,  real_t* fy,  real_t* fz,
    __global const real_t* coordA,
    __global const real_t* coordB,
             const real_t  massA,
             const real_t  massB,
             const real_t  noForceDistance
) {
    // расстояние
    real_t rx = coordB[ 0 ] - coordA[ 0 ];
    real_t ry = coordB[ 1 ] - coordA[ 1 ];
    real_t rz = coordB[ 2 ] - coordA[ 2 ];
    const real_t distance = sqrt( rx * rx + ry * ry + rz * rz );
    if (distance <= noForceDistance) {
        // при столкновениях отключаем силу
        return;
    }
    if (distance > 0.0) {
        const real_t inv = 1.0 / distance;
        rx *= inv;
        ry *= inv;
        rz *= inv;
    }

    // сила гравитации
    const real_t f = G * massA * massB / (distance * distance);
    *fx = rx * f;
    *fy = ry * f;
    *fz = rz * f;
}




inline void planetInteraction(
    __global       aboutPlanet_t*      wp,
    __global const aboutPlanet_t*      p,
             const uint                i,
    __global const aboutStarSystem_t*  ass,
    __global const aboutBody_t*        ab,
             const real_t              timestep
) {
    // рассчитываем результирующую силу
    real_t fx, fy, fz;
    fx = fy = fz = 0.0;

    for (uint k = 0; k < BODY_COUNT; ++k) {
        if (i == k) {
            // сама на себя не действует
            continue;
        }

        // силы гравитации, действующие на планету со стороны
        // тела с индексом 'k'

        const enum GROUP_ELEMENT group = ab[ k ].group;
        switch ( group ) {
            case GE_PLANET:
                forceGravityBodyBodyInteraction(
                    &fx, &fy, &fz,
                    p->coord,  ab[ k ].content.planet.coord,
                    p->mass,   ab[ k ].content.planet.mass,
                    // на этом расстоянии силы не действуют
                    max( p->radius,  ab[ k ].content.planet.radius )
                );
                break;

            case GE_STAR:
                forceGravityBodyBodyInteraction(
                    &fx, &fy, &fz,
                    p->coord,  ab[ k ].content.star.coord,
                    p->mass,   ab[ k ].content.star.mass,
                    // на этом расстоянии силы не действуют
                    max( p->radius,  ab[ k ].content.star.radius )
                );
                break;

            default:
                // другие тела не действуют на планету
                continue;
        }

    } // for (uint k = 0; k < BODY_COUNT; ++k)


    // запоминаем силу
    wp->force[ 0 ] = fx;
    wp->force[ 1 ] = fy;
    wp->force[ 2 ] = fz;

    // новая скорость
    const real_t f = sqrt( fx * fx + fy * fy + fz * fz );
    if (f > 0.0) {
        const real_t inv = 1.0 / f;
        fx *= inv;
        fy *= inv;
        fz *= inv;
    }
    const real_t acceleration = f / p->mass;
    const real_t v = acceleration * timestep;
    wp->velocity[ 0 ] += fx * v;
    wp->velocity[ 1 ] += fy * v;
    wp->velocity[ 2 ] += fz * v;

    // новые координаты
    wp->coord[ 0 ] += p->velocity[ 0 ] * timestep;
    wp->coord[ 1 ] += p->velocity[ 1 ] * timestep;
    wp->coord[ 2 ] += p->velocity[ 2 ] * timestep;
    
    /* @test*/
    wp->test[ 1 ] = f;
    wp->test[ 2 ] = acceleration;
    wp->test[ 3 ] = v;
    /**/
}





inline void starInteraction(
    __global       aboutStar_t*        ws,
    __global const aboutStar_t*        s,
             const uint                i,
    __global const aboutStarSystem_t*  ass,
    __global const aboutBody_t*        ab,
             const real_t              timestep
) {
    // рассчитываем результирующую силу
    real_t fx, fy, fz;
    fx = fy = fz = 0.0;

    for (uint k = 0; k < BODY_COUNT; ++k) {
        if (i == k) {
            // сама на себя не действует
            continue;
        }

        // силы гравитации, действующие на звезду со стороны
        // тела с индексом 'k'

        const enum GROUP_ELEMENT group = ab[ k ].group;
        switch ( group ) {
            case GE_PLANET:
                forceGravityBodyBodyInteraction(
                    &fx, &fy, &fz,
                    s->coord,  ab[ k ].content.planet.coord,
                    s->mass,   ab[ k ].content.planet.mass,
                    // на этом расстоянии силы не действуют
                    max( s->radius,  ab[ k ].content.planet.radius )
                );
                break;

            case GE_STAR:
                forceGravityBodyBodyInteraction(
                    &fx, &fy, &fz,
                    s->coord,  ab[ k ].content.star.coord,
                    s->mass,   ab[ k ].content.star.mass,
                    // на этом расстоянии силы не действуют
                    max( s->radius,  ab[ k ].content.star.radius )
                );
                break;

            default:
                // другие тела не действуют на звезду
                continue;
        }

    } // for (uint k = 0; k < BODY_COUNT; ++k)


    // запоминаем силу
    ws->force[ 0 ] = fx;
    ws->force[ 1 ] = fy;
    ws->force[ 2 ] = fz;

    // новая скорость
    const real_t f = sqrt( fx * fx + fy * fy + fz * fz );
    if (f > 0.0) {
        const real_t inv = 1.0 / f;
        fx *= inv;
        fy *= inv;
        fz *= inv;
    }
    const real_t acceleration = f / s->mass;
    const real_t v = acceleration * timestep;
    ws->velocity[ 0 ] += fx * v;
    ws->velocity[ 1 ] += fy * v;
    ws->velocity[ 2 ] += fz * v;

    // новые координаты
    ws->coord[ 0 ] += s->velocity[ 0 ] * timestep;
    ws->coord[ 1 ] += s->velocity[ 1 ] * timestep;
    ws->coord[ 2 ] += s->velocity[ 2 ] * timestep;
    
    /* @test
    ws->test[ 1 ] = f;
    ws->test[ 2 ] = acceleration;
    ws->test[ 3 ] = v;
    */
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
    
    // @test
    wab[ i ].content.planet.mass *= 2.0;
    //wab[ i ].content.planet.test[ 1 ] += 1e4;
    return;

    // # Поля для физ. тел. могут располагаться в любом порядке.
    // @todo bad optimize Утверждённый порядок = возможность оптимизации.
    switch ( group ) {
        case GE_PLANET:
            planetInteraction(
                &wab[ i ].content.planet,
                &ab[ i ].content.planet,
                i, ass, ab, timestep
            );

            // @test
            //wab[ i ].content.planet.coord[ 1 ] += 1e4;

            break;

        case GE_STAR:
            starInteraction(
                &wab[ i ].content.star,
                &ab[ i ].content.star,
                i, ass, ab, timestep
            );
            break;

        default:
            // не обрабатываем другие элементы
            break;
    }

}
