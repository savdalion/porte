// @require Библиотеки из "include".


/**
* Фиксирует характеристики элемента звёздной системы согласно событиям.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядра direct(), relative().
*
* #i NVIDIA 8800GTS работает только с float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void fix(
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    const real_t                       timestep  // 4
) {
    return;

    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );

    if (i >= STAR_COUNT) {
        //printf( "(!) Index %d / %d out of range for star.\n",  i,  STAR_COUNT - 1 );
        return;
    }

    if ( absentStar( &as[ i ] ) ) {
        return;
    }


    __global aboutStar_t* element = &as[ i ];
    const __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        //printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
        //    element->uid, ee->waldo );
    }
#endif


    // меняем характеристики э. в зависимости от событий
    // # Не игнорируем ни единого события: ведь они почему-то
    //   оказались у этого элемента.
    for (int w = ee->waldo - 1; w >= 0; --w) {
        __global const eventTwo_t* e = &ee->content[ w ];
        const enum EVENT uid = e->uid;
        // # Упорядочено согласно enum Event.
        switch ( uid ) {
            case E_NONE :
                // пустая ячейка, пустое событие
                // # В идеале не должно встречаться.
#ifdef __DEBUG
                //printf( "(?) fix() Empty event for asteroid %d. Decrease perfomance.\n", element->uid );
#endif
                break;

            case E_COLLISION :
                // # Составное событие.
                // # Событие исключительно для слушателей.
                break;

            case E_CHANGE_COORD :
                element->today.coord.x += e->fReal[ 0 ];
                element->today.coord.y += e->fReal[ 1 ];
                element->today.coord.z += e->fReal[ 2 ];
#ifdef __DEBUG
                assertReal( element->today.coord.x, "(!) Overfill coord X for star.\n" );
                assertReal( element->today.coord.y, "(!) Overfill coord Y for star.\n" );
                assertReal( element->today.coord.z, "(!) Overfill coord Z for star.\n" );
#endif
                break;

            case E_CHANGE_MASS :
                element->today.mass += e->fReal[ 1 ];
#ifdef __DEBUG
                assertReal( element->today.mass, "(!) Overfill mass for star.\n" );
#endif
                break;

            case E_CHANGE_SIZE :
                // @todo ...
                break;

            case E_CHANGE_TEMPERATURE :
                element->today.surfaceTemperature += e->fReal[ 1 ];
                break;

            case E_CHANGE_VELOCITY :
                element->today.velocity.x += e->fReal[ 0 ];
                element->today.velocity.y += e->fReal[ 1 ];
                element->today.velocity.z += e->fReal[ 2 ];
#ifdef __DEBUG
                assertReal( element->today.velocity.x, "(!) Overfill velocity X for star.\n" );
                assertReal( element->today.velocity.y, "(!) Overfill velocity Y for star.\n" );
                assertReal( element->today.velocity.z, "(!) Overfill velocity Z for star.\n" );
#endif
                break;

            case E_CRUSH_N :
                // @todo ...
                break;

            case E_DESTROY :
#ifdef __DEBUG
                //printf( "fix() Destroy star %d.\n", element->uid );
#endif
                element->today.live = false;
                break;

            case E_GRAVITY :
                // # Гравитация звезды на себя не влияет, только на элементы
                //   звёздной системы.
                // # Событие исключительно для слушателей.
                break;

            case E_IMPACT_ACCELERATION :
                // # Событие исключительно для слушателей.
                break;

            case E_IMPACT_FORCE :
                // # Событие исключительно для слушателей.
                break;

            case E_RADIATION :
                // # Потеря массы при излучении уже было представлено как
                //   отдельное событие в ядре direct() и будет отработано как
                //   E_CHANGE_MASS.
                // # Событие исключительно для слушателей.
                break;

            case E_last :
                // просто должны указать по требованию компилятора
                // # Не должно встречаться.
                break;

            //default :
                // # Сюда попадать не должны.
#ifdef __DEBUG
                //printf( "(!) fix() Undefined event for star %d.\n", element->uid );
#endif
        } // switch

    } // for (int w = ...

}
