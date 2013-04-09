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
    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );

    if (i >= ASTEROID_COUNT) {
        //printf( "(!) Index %d / %d out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
        return;
    }

    if ( absentAsteroid( &aa[ i ] ) ) {
        return;
    }


    __global aboutAsteroid_t* element = &aa[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        //printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n",
        //    element->uid, ee->waldo );
    }
#endif


    // обнуляем события
    /* - optimize Достаточно обнулить вальдо.
    for (int w = ee->waldo; w > 0; --w) {
        ee->content[ w ].uid = E_NONE;
    }
    */

    // обнуляем вальдо
    ee->waldo = 0;

}
