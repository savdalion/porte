// @require Библиотеки из "include".


/**
* Взаимодействие элементов звёздной системы.
* Реализовано с помощью событий.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
*
* #i NVIDIA 8800GTS работает только с float.
*/




#if 0
inline void asteroidEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as
) {
    /* @test
    int w = aa->emitterEvent.waldo;
    for (uint k = 0; k < STAR_COUNT; ++k) {
        __global const aboutStar_t* ask = sc[ k ];
        const bool absent = absentStar( ask );
        if ( absent ) {
            // # Отсутствующий элемент - сигнал конца списка.
            return;
        }
    }
    return;
    */


    // обязательные события для астероида
    // отсутствуют
    int w = aa->emitterEvent.waldo;


    // проверка на столкновения
    const real_t coordA[ 3 ] =
        { aa->today.coord[ 0 ],  aa->today.coord[ 1 ],  aa->today.coord[ 2 ] };
    const real_t maxSide =
        max( max( aa->today.size[ 0 ], aa->today.size[ 1 ] ), aa->today.size[ 2 ] );

    for (uint k = 0; k < STAR_COUNT; ++k) {
        __global const aboutStar_t* ask = sc[ k ];
        
        // @test
        aa->emitterEvent.waldo = 1;
        return;
        
        if ( !absentStar( ask ) ) {
            // # Отсутствующий элемент - сигнал конца списка.
            // break;  - @todo Зависает. Причина?
            const real_t coordB[ 3 ] =
                { ask->today.coord[ 0 ],  ask->today.coord[ 1 ],  ask->today.coord[ 2 ] };
            const real_t collisionDistance = max( maxSide, ask->today.radius );
            const bool hasCollision =
                collision( coordA,  coordB,  collisionDistance );
            //if ( hasCollision )
            {
                /*
                eventTwo_t e = {
                    // uid события
                    E_COLLISION,
                    // второй участник события
                    { GE_STAR,  k,  ask->uid },
                    {}
                };
                */
                //aa->emitterEvent.content[ w ] = e;  ++w;
                //++w;
                //aa->emitterEvent.content[ 1 ].uid = 0;
                aa->emitterEvent.waldo = 1;
            }
        }

    } // for (size_t k = 0 ...

}




inline void starEmitEvent(
    __global aboutStar_t* as
) {
    // обязательные события для звезды
    int w = as->emitterEvent.waldo;
    const aboutStar_t star = *as;
    
    // гравитация
    const real_t gm = G * massStar( &star );
    const eventTwo_t e = {
        // enum EVENT
        E_GRAVITY,
        // pointerElement_t - здесь не важен
        { GE_NONE },
        // real_t fReal[ MAX_FEATURE_EVENT ]
        { gm }
    };
    as->emitterEvent.content[ w ] = e;  ++w;
    
    // проверка на столкновения
    // # Проверяем только элемент, идущий первым названием по алфавиту.
    //   В данном случае STAR уже проверена ASTEROID, PLANET.

    // фиксируем вальдо
    as->emitterEvent.waldo = w;
}
#endif



// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void emit(
    __global const aboutStarSystem_t*  ass,  // 0
    __global aboutAsteroid_t*          aa,   // 1
    __global aboutPlanet_t*            ap,   // 2
    __global aboutStar_t*              as    // 3
) {
    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );
    
    // @test
    aa[ i ].uid = sizeof( aboutAsteroid_t );
    aa[ i ].tf1 = 8.8f;
    aa[ i ].tf2 = 7.7f;
    aa[ i ].td1 = (double)6.6;
    aa[ i ].td2 = (double)5.5;
    aa[ i ].emitterEvent.waldo = 7;
    if (i < ASTEROID_COUNT) {
        aa[ i ].emitterEvent.waldo = 7;
    }
    if (i < PLANET_COUNT) {
        ap[ i ].emitterEvent.waldo = 123;
    }
    if (i < STAR_COUNT) {
        as[ i ].emitterEvent.waldo = 456;
    }
    return;


    if (i < ASTEROID_COUNT) {
        //asteroidEmitEvent( ac[ i ], ac, pc, sc, i );
    }
    
    if (i < PLANET_COUNT) {
        //planetEmitEvent( pc[ i ] );
    }
    
    if (i < STAR_COUNT) {
        //starEmitEvent( sc[ i ] );
    }

}
