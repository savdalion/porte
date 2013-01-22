// @require Библиотеки из "include".


/**
* Подготавливает элементы звёздной системы к циклу испускания событий.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядра direct(), relative(), fix().
*
* #i NVIDIA 8800GTS работает только с float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void begin(
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    const real_t                       timestep  // 4
) {
    /* @test
    printf( "aboutStarSystem_t %i\n", sizeof( aboutStarSystem_t ) );
    printf( "aboutAsteroid_t %i\n", sizeof( aboutAsteroid_t ) );
    printf( "aboutPlanet_t %i\n", sizeof( aboutPlanet_t ) );
    printf( "aboutStar_t %i\n", sizeof( aboutStar_t ) );
    printf( "real_t %i\n", sizeof( real_t ) );
    printf( "real2_t %i\n", sizeof( real2_t ) );
    printf( "real4_t %i\n", sizeof( real4_t ) );
    printf( "characteristicAsteroid_t %i\n", sizeof( characteristicAsteroid_t ) );
    printf( "bool %i\n", sizeof( bool ) );
    printf( "eventTwo_t %i\n", sizeof( eventTwo_t ) );
    printf( "pointerElement_t %i\n", sizeof( pointerElement_t ) );
    printf( "enum EVENT %i\n", sizeof( enum EVENT ) );
    return;
    */


    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );

    if ( (i > STAR_COUNT) || absentStar( &as[ i ] ) ) {
        return;
    }


    __global aboutStar_t* element = &as[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }
#endif


    // обнуляем события
    // @todo optimize Достаточно обнулить вальдо.
    for (int w = ee->waldo; w > 0; --w) {
        ee->content[ w ].uid = E_NONE;
    }

    // обнуляем вальдо
    ee->waldo = 0;

}
