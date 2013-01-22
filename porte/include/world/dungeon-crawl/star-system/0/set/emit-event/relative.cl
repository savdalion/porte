// @require Библиотеки из "include".


/**
* Зависимые события от элементов звёздной системы.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядро direct().
*
* #i NVIDIA 8800GTS работает только с float.
*/




inline void asteroidRelativeEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutAsteroid_t* aai = &aa[ i ];
    int w = aai->emitterEvent.waldo;
#ifdef __DEBUG
    if ( !betweenInteger( w, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n", aai->uid, w );
    }
#endif


    // события, которые могут повлиять на астероид


    // Проверка на столкновения
    // # Проверено в asteroidDirectEmitEvent(). Астероид сам является
    //   источником этого события.


    // Силы от других элементов
    // накопим результат, просмотрев элементы звёздной системы
    real4_t gfA = (real4_t)( 0 );

    const real4_t coordA = (real4_t)(
        convertFromBigValue( aai->today.coord.x ),
        convertFromBigValue( aai->today.coord.y ),
        convertFromBigValue( aai->today.coord.z ),
        0
    );
    const real_t massA = massAsteroid( aai );

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
        aai->emitterEvent.content[ w ] = e;
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
        aai->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Меняется скорость
    // @todo optimize Не учитывать мизерные изменения.
    const real4_t deltaVelocityA = accelerationA * timestep;
    const real4_t velocityA = (real4_t)(
        aai->today.velocity[ 0 ],
        aai->today.velocity[ 1 ],
        aai->today.velocity[ 2 ],
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
            aai->emitterEvent.content[ w ] = e;
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
            aai->emitterEvent.content[ w ] = e;
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
        aai->emitterEvent.content[ w ] = e;
        ++w;
    }


    // обновляем вальдо
    aai->emitterEvent.waldo = w;
}








inline void starRelativeEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutStar_t* asi = &as[ i ];
    int w = asi->emitterEvent.waldo;
#ifdef __DEBUG
    if ( !betweenInteger( w, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n", asi->uid, w );
    }
#endif


    // события, которые могут повлиять на звезду
    const pointerElement_t piA = { GE_STAR, i, asi->uid };

    // Столкновение с другими элементами
    // # Симметричное событие. Ловится астероидами (т.к. идут по
    //   алфавиту раньше STAR).

    // Столкновение с астероидами
    {
        for (uint k = 0;
            // # Здесь можем воспользоваться признаком "Пустой элемент -
            //   дальше нет событий", т.к. элементы в начале каждого
            //   пульса - упорядочены. См. соглашение в начале пульса.
            (k < ASTEROID_COUNT) && presentAsteroid( &aa[ k ] );
            ++k
        ) {
            __global const aboutAsteroid_t* aak = &aa[ k ];
            __global const emitterEvent_t* ee = &aak->emitterEvent;
            for (int we = ee->waldo - 1; we >= 0; --we) {
                __global const eventTwo_t* ewe = &ee->content[ we ];
                if ( (ewe->uid == E_COLLISION)
                  && accordancePointerElement( GE_STAR, i, asi->uid,  &ewe->pi )
                ) {
#ifdef __DEBUG
                    printf( "relative() Star %d collision with asteroid %d.\n", asi->uid, aak->uid );
#endif
                    const pointerElement_t piB = { GE_ASTEROID, k, aak->uid };
                    if (w < EMITTER_EVENT_COUNT) {
                        const real_t kineticABefore = ewe->fReal[ 0 ];
                        const real_t kineticBBefore = ewe->fReal[ 1 ];
                        const real_t deltaKineticA  = ewe->fReal[ 2 ];
                        const real_t deltaKineticB  = ewe->fReal[ 3 ];
                        eventTwo_t e = {
                            // uid события
                            E_COLLISION,
                            // второй участник события
                            piB,
                            {
                                // энергии для события уже посчитаны, только
                                // обменяем их местами
                                kineticBBefore, kineticABefore,
                                deltaKineticB,  deltaKineticA
                            }
                        };
                        asi->emitterEvent.content[ w ] = e;
                        ++w;
                    }
                    
                    // столкновение с астероидом повлечёт для звезды и др. события

                    // Увеличение массы от падения астероида
                    const real_t deltaMassBySecond = massAsteroid( aak );
                    const real_t deltaMassByTimestep = deltaMassBySecond;
                    if (w < EMITTER_EVENT_COUNT) {
                        eventTwo_t e = {
                            // uid события
                            E_CHANGE_MASS,
                            // второй участник события - почему бы и нет
                            piB,
                            // # Необходимо соблюдать порядок передачи
                            //   параметров для одинаковых событий.
                            { deltaMassBySecond, deltaMassByTimestep }
                        };
                        asi->emitterEvent.content[ w ] = e;
                        ++w;
                    }

                    if (w < EMITTER_EVENT_COUNT) {
                        eventTwo_t e = {
                            // uid события
                            E_INCREASE_MASS,
                            // второй участник события - почему бы и нет
                            piB,
                            // # Необходимо соблюдать порядок передачи
                            //   параметров для одинаковых событий.
                            { deltaMassBySecond, deltaMassByTimestep }
                        };
                        asi->emitterEvent.content[ w ] = e;
                        ++w;
                    }

                } // if ( (ewe->uid == E_COLLISION) && ...

            } // for (int we = ...

        } // for (uint k = ...

    } // Столкновение с астероидами


    // обновляем вальдо
    asi->emitterEvent.waldo = w;
}




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


    if ( (i < ASTEROID_COUNT) && presentAsteroid( &aa[ i ] ) ) {
        asteroidRelativeEmitEvent( i, aa, ap, as, timestep );
    }

    if ( (i < PLANET_COUNT) && presentPlanet( &ap[ i ] ) ) {
        //planetRelativeEmitEvent( pc[ i ] );
    }

    if ( (i < STAR_COUNT) && presentStar( &as[ i ] ) ) {
        starRelativeEmitEvent( i, aa, ap, as, timestep );
    }

}
