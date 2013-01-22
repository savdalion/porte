// @require Библиотеки из "include".


/**
* Зависимые события от элементов звёздной системы.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядро direct().
*
* #i NVIDIA 8800GTS работает только с float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void relative(
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


    // Проверка на столкновения
    // # Проверено в asteroidDirectEmitEvent(). Астероид сам является
    //   источником этого события.


    // Силы от других элементов
    // накопим результат, просмотрев элементы звёздной системы
    real4_t gfA = (real4_t)( 0 );

    const real4_t coordA = (real4_t)(
        convertFromBigValue( element->today.coord.x ),
        convertFromBigValue( element->today.coord.y ),
        convertFromBigValue( element->today.coord.z ),
        0
    );
    const real_t massA = massAsteroid( element );

    // # Отсутствующий элемент - сигнал конца списка.
    // # Прекращаем запоминать события, если память переполнена.

    // Гравитация от звёзд
    {
        for (uint k = 0;
            // # Здесь можем воспользоваться признаком "Пустой элемент -
            //   дальше нет событий", т.к. элементы в начале каждого
            //   пульса - упорядочены. См. соглашение в начале пульса.
            (k < STAR_COUNT) && presentStar( &as[ k ] );
            ++k
        ) {
            __global const aboutStar_t* ask = &as[ k ];
            __global const emitterEvent_t* ee = &ask->emitterEvent;
            for (int we = ee->waldo - 1; we >= 0; --we) {
                // @todo optimize Не учитывать мизерные воздействия.
                if (ee->content[ we ].uid == E_GRAVITY) {
                    // часть формулы уже вычислена звездой
                    const real_t fgm = ee->content[ we ].fReal[ 0 ];
                    const real4_t coordB = (real4_t)(
                        ask->today.coord[ 0 ],
                        ask->today.coord[ 1 ],
                        ask->today.coord[ 2 ],
                        0
                    );
                    const real4_t dc = coordB - coordA;
                    const real4_t squareDC = dc * dc;
                    const real_t lengthDC = lengthVector( dc );
                    if ( !zero( lengthDC ) ) {
                        const real4_t normalDC = dc / lengthDC;
                        // f = (G * mB) * mA / distance ^ 2
                        // @todo optimize Вынести '* massA' за циклы.
                        const real_t fgmm = fgm * massA;
                        gfA += (real4_t)(
                            zero( normalDC.s0 ) ? 0 : (fgmm / squareDC.s0),
                            zero( normalDC.s1 ) ? 0 : (fgmm / squareDC.s1),
                            zero( normalDC.s2 ) ? 0 : (fgmm / squareDC.s2),
                            0
                        ) * normalDC;
                        // # В списке событий от 1 элемента может быть только
                        //   одна гравитационная сила.
                        break;
                    }
                }

            } // for (int we = ...

        } // for (uint k = ...

    } // Гравитация от звёзд



    // Результирующая сила
    // @todo optimize Не учитывать мизерные воздействия.
    const real_t absForceA = lengthVector( gfA );
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_IMPACT_FORCE,
            // второй участник события - здесь не важен
            {},
            { gfA.s0, gfA.s1, gfA.s2,  absForceA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Действует ускорение
    // @todo optimize Не учитывать мизерные ускорения.
    const real4_t accelerationA = gfA / massA;
    const real_t absAccelerationA = absForceA / massA;  // = lengthVector( accelerationA )
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_IMPACT_ACCELERATION,
            // второй участник события - здесь не важен
            {},
            { accelerationA.s0, accelerationA.s1, accelerationA.s2,  absAccelerationA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Меняется скорость
    // @todo optimize Не учитывать мизерные изменения.
    const real4_t deltaVelocityA = accelerationA * timestep;
    const real4_t velocityA = (real4_t)(
        element->today.velocity[ 0 ],
        element->today.velocity[ 1 ],
        element->today.velocity[ 2 ],
        0
    );
    const real_t absVelocityA = lengthVector( velocityA );
    const real4_t futureVelocityA = velocityA + deltaVelocityA;
    const real_t absFutureVelocityA = lengthVector( futureVelocityA );
    const real_t absDeltaVelocityA = absFutureVelocityA - absVelocityA;
    // # Скорость могла остаться прежней. Бережём память э. для др. событий.
    if ( !zero( absDeltaVelocityA ) ) {
        // @todo optimize? Ниже - повторение. Убрать его - ускорит обработку?
        // Общее событие об изменении скорости
        if (w < EMITTER_EVENT_COUNT) {
            eventTwo_t e = {
                // uid события
                E_CHANGE_VELOCITY,
                // второй участник события - здесь не важен
                {},
                { deltaVelocityA.s0, deltaVelocityA.s1, deltaVelocityA.s2, absDeltaVelocityA }
            };
            element->emitterEvent.content[ w ] = e;
            ++w;
        }

        // Как изменилась скорость по отношению к текущей
        if (w < EMITTER_EVENT_COUNT) {
            eventTwo_t e = {
                // uid события
                (absDeltaVelocityA < 0) ? E_DECREASE_VELOCITY : E_INCREASE_VELOCITY,
                // второй участник события - здесь не важен
                {},
                { deltaVelocityA.s0, deltaVelocityA.s1, deltaVelocityA.s2,  absDeltaVelocityA }
            };
            element->emitterEvent.content[ w ] = e;
            ++w;
        }
    } // if ( !zero( absDeltaVelocityA ) )


    // Меняются координаты
    // @todo optimize Не учитывать мизерные изменения.
    const real4_t deltaCoordA = futureVelocityA * timestep;
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_CHANGE_COORD,
            // второй участник события - здесь не важен
            {},
            { deltaCoordA.s0, deltaCoordA.s1, deltaCoordA.s2 }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // обновляем вальдо
    ee->waldo = w;

}
