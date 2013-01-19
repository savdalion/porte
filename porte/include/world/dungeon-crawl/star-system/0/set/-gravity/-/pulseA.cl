// @require Библиотеки из "include".


/**
* Взаимодействие физических тел в полях гравитации (область
* звёздной системы). 
* Рассчитывает взаимодействия, результаты сохраняются во временной таблице.
*
* @todo fine Переписать на double. NVIDIA 8800GTS работает только с float.
*/



/**
* Действие планеты B на планету A.
*/
inline void planetPlanetInteraction(
    __global       aboutPlanet_t*  wa,
    __global const aboutPlanet_t*  a,
    __global const aboutPlanet_t*  b,
             const float           timestep
) {
    // @todo optimize Можно существенно оптимизировать. См. проект
    //       OpenCL / NBody от NVIDIA.

    // расстояние
    const float4 r = (float4)(
        a->coord[ 0 ] - b->coord[ 0 ],
        a->coord[ 1 ] - b->coord[ 1 ],
        a->coord[ 2 ] - b->coord[ 2 ],
        0.0
    );
    const float distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    const float distance = rsqrt( distanceSquared );

    // сила гравитации
    const float forceWithoutMassA = G * b->mass / distanceSquared;
    const float force = forceWithoutMassA * a->mass;
    const float4 rn = r / distance;
    wa->force[ 0 ] += rn.x * force;
    wa->force[ 1 ] += rn.y * force;
    wa->force[ 2 ] += rn.z * force;

    // скорость
    //const float acceleration = force / a->mass;
    const float acceleration = forceWithoutMassA;
    const float velocity = acceleration * timestep;
    wa->velocity[ 0 ] += rn.x * velocity;
    wa->velocity[ 1 ] += rn.y * velocity;
    wa->velocity[ 2 ] += rn.z * velocity;
}




/**
* Действие звезды B на планету A.
*/
inline void planetStarInteraction(
    __global       aboutPlanet_t*  wa,
    __global const aboutPlanet_t*  a,
    __global const aboutStar_t*    b,
             const float           timestep
) {
    // @todo optimize Можно существенно оптимизировать. См. проект
    //       OpenCL / NBody от NVIDIA.

    // расстояние
    const float4 r = (float4)(
        a->coord[ 0 ] - b->coord[ 0 ],
        a->coord[ 1 ] - b->coord[ 1 ],
        a->coord[ 2 ] - b->coord[ 2 ],
        0.0
    );
    const float distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    const float invertDistance = rsqrt( distanceSquared );

    // сила гравитации
    const float forceWithoutMassA = G * b->mass / distanceSquared;
    const float force = forceWithoutMassA * a->mass;
    const float4 rn = r * invertDistance;
    wa->force[ 0 ] += rn.x * force;
    wa->force[ 1 ] += rn.y * force;
    wa->force[ 2 ] += rn.z * force;

    // скорость
    //const float acceleration = force / a->mass;
    const float acceleration = forceWithoutMassA;
    const float velocity = acceleration * timestep;
    wa->velocity[ 0 ] += rn.x * velocity;
    wa->velocity[ 1 ] += rn.y * velocity;
    wa->velocity[ 2 ] += rn.z * velocity;
}




/**
* Рассчитывает взаимодействие планеты с телами звёздной системы.
*/
inline void planetInteraction(
    __global       aboutPlanet_t*      wap,
    __global const aboutPlanet_t*      p,
             const uint                i,
    __global const aboutStarSystem_t*  ass,
    __global const aboutBody_t*        ab,
             const float               timestep
) {
    // рассчитываем силы и скорости
    // значения сил обнуляем на каждой итерации
    // @todo optimize Накапливать значения в __local-переменной.
    wap->force[ 0 ] =
        wap->force[ 1 ] =
        wap->force[ 2 ] = 0.0;

    for (uint k = 0; k < BODY_COUNT; ++k) {
        if (k == i) {
            continue;
        }

        // силы гравитации, действующие на планету со стороны
        // тела с индексом 'k'

        const enum GROUP_ELEMENT group = ab[ k ].group;
        switch ( group ) {
            case GE_PLANET:
                //planetPlanetInteraction( wap, p, &ab[ k ].content.planet, timestep );
                break;

            case GE_STAR:
                planetStarInteraction( wap, p, &ab[ k ].content.star, timestep );
                break;

            default:
                // другие тела не действуют на планету
                continue;
        }

    } // for (uint k = 0; k < BODY_COUNT; ++k)


    // вычисляем новые координаты
    wap->coord[ 0 ] += wap->velocity[ 0 ] * timestep;
    wap->coord[ 1 ] += wap->velocity[ 1 ] * timestep;
    wap->coord[ 2 ] += wap->velocity[ 2 ] * timestep;
}





// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void pulseA(
    __global const aboutStarSystem_t*  ass,      // 0
    __global const aboutBody_t*        ab,       // 1
    __global       aboutBody_t*        wab,      // 2
             const float               timestep  // 3
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
            return;
    }

}
