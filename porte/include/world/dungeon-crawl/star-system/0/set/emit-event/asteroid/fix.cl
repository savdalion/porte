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
    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );

    if ( (i > ASTEROID_COUNT) || absentAsteroid( &aa[ i ] ) ) {
        return;
    }

    __global aboutAsteroid_t* element = &aa[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


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
                printf( "(?) fix() Empty event for asteroid %d. Decrease perfomance.\n", element->uid );
#endif
                break;

            case E_COLLISION :
                // # Составное событие.
                // # Событие исключительно для слушателей.
                break;

            case E_CHANGE_COORD :
                element->today.coord.x += convertToBigValue( e->fReal[ 0 ] );
                element->today.coord.y += convertToBigValue( e->fReal[ 1 ] );
                element->today.coord.z += convertToBigValue( e->fReal[ 2 ] );
                break;

            case E_CHANGE_MASS :
                element->today.mass += convertToBigValue( e->fReal[ 1 ] );
                break;

            case E_CHANGE_TEMPERATURE :
                element->today.temperature += e->fReal[ 1 ];
                break;

            case E_CHANGE_VELOCITY :
                element->today.velocity[ 0 ] += e->fReal[ 0 ];
                element->today.velocity[ 1 ] += e->fReal[ 1 ];
                element->today.velocity[ 2 ] += e->fReal[ 2 ];
                break;

            case E_DECREASE_MASS :
                // # Отрабатывается в E_CHANGE_MASS.
                break;

            case E_DECREASE_TEMPERATURE :
                // # Отрабатывается в E_CHANGE_TEMPERATURE.
                break;

            case E_DECREASE_VELOCITY :
                // # Отрабатывается в E_CHANGE_VELOCITY.
                break;

            case E_INCREASE_MASS :
                // # Отрабатывается в E_CHANGE_MASS.
                break;

            case E_INCREASE_TEMPERATURE :
                // # Отрабатывается в E_CHANGE_TEMPERATURE.
                break;

            case E_INCREASE_VELOCITY :
                // # Отрабатывается в E_CHANGE_VELOCITY.
                break;

            case E_CRUSH_N :
                // @todo ...
                break;

            case E_DESTROY :
#ifdef __DEBUG
                printf( "fix() Destroy asteroid %d.\n", element->uid );
#endif
                element->today.live = false;
                break;

            case E_GRAVITY :
                // # Астероид слишком мал, чтобы гравитационно воздействовать
                //   на другие элементы.
                // # Событие исключительно для слушателей.
                break;

            case E_IMPACT_ACCELERATION :
                // # Событие исключительно для слушателей.
                break;

            case E_IMPACT_FORCE :
                // # Событие исключительно для слушателей.
                break;

            case E_RADIATION :
                // # Событие исключительно для слушателей.
                break;

            case E_last :
                // просто должны указать по требованию компилятора
                // # Не должно встречаться.
                break;

            default :
                // # Сюда попадать не должны.
#ifdef __DEBUG
                printf( "(!) fix() Undefined event for asteroid %d.\n", element->uid );
#endif
        } // switch

    } // for (int w = ...

}
