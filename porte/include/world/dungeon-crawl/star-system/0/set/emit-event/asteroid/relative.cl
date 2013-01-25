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
    return;

    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );

    if (i >= ASTEROID_COUNT) {
        printf( "(!) Index %d / %d out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
        return;
    }

    if ( absentAsteroid( &aa[ i ] ) ) {
        return;
    }


    __global aboutAsteroid_t* element = &aa[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }

    printf( "Asteroid %d.\n"
        "Coord %e %e %e\n"
        "Velocity %e %e %e\n"
        "Size %e %e %e\n"
        "",
        element->uid,
        element->today.coord.x,
        element->today.coord.y,
        element->today.coord.z,
        element->today.velocity[ 0 ],
        element->today.velocity[ 1 ],
        element->today.velocity[ 2 ],
        element->today.size[ 0 ],
        element->today.size[ 1 ],
        element->today.size[ 2 ]
    );
#endif

    int w = ee->waldo;


    // Проверка на столкновения
    // # Проверено в asteroidDirectEmitEvent(). Астероид сам является
    //   источником этого события.


    // Силы от других элементов
    // накопим результат, просмотрев элементы звёздной системы
    real4_t gfA = (real4_t)( 0 );

    real4_t coordA;
    convertFromBig3DValue( &coordA, element->today.coord );
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
                    real4_t coordB;
                    convertFromBig3DValue( &coordB, ask->today.coord);
                    const real4_t dc = coordB - coordA;
                    const real4_t squareDC = dc * dc;
                    const real_t lengthDC = lengthVectorAccurate( dc );
#ifdef __DEBUG
                    /* - Увидим ошибку при проверке силы, ниже.
                    assertReal( lengthDC, "(!) Overfill distance between asteroid and star.\n" );
                    */
#endif
                    const bool correctLengthDC = testReal4( lengthDC );
                    if ( correctLengthDC && !zero( lengthDC ) ) {
                        const real4_t normalDC = dc / lengthDC;
                        // f = (G * mB) * mA / distance ^ 2
                        // @todo optimize Вынести '* massA' за циклы.
                        const real_t fgmm = fgm * massA;
                        // @todo optimize Использовать деление для real4_t.
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
    const real_t absForceA = lengthVectorAccurate( gfA );
    const bool correctAbsForce = testReal( absForceA );
#ifdef __DEBUG
    if ( !correctAbsForce ) {
        printf( "Asteroid %d.\n"
            "Coord %e %e %e\n"
            "Velocity %e %e %e\n"
            "Size %e %e %e\n"
            "",
            element->uid,
            element->today.coord.x,
            element->today.coord.y,
            element->today.coord.z,
            element->today.velocity[ 0 ],
            element->today.velocity[ 1 ],
            element->today.velocity[ 2 ],
            element->today.size[ 0 ],
            element->today.size[ 1 ],
            element->today.size[ 2 ]
        );
    }
    assertReal4( gfA, "(!) Overfill E_IMPACT_FORCE for asteroid.\n" );
    assertReal( absForceA, "(!) Overfill length E_IMPACT_FORCE for asteroid.\n" );
#endif
    if ( correctAbsForce && (w < EMITTER_EVENT_COUNT) ) {
        const eventTwo_t e = {
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
        const eventTwo_t e = {
            // uid события
            E_IMPACT_ACCELERATION,
            // второй участник события - здесь не важен
            {},
            // @todo optimize fine Хранить значения в виде 4D-векторов.
            { accelerationA.s0, accelerationA.s1, accelerationA.s2,  absAccelerationA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Меняется скорость
    // @todo optimize Не учитывать мизерные изменения.
    const real4_t deltaVelocityA = accelerationA * timestep;
    const real4_t velocityA = element->today.velocity;
    const real_t absVelocityA = lengthVector( velocityA );
    const real4_t futureVelocityA = velocityA + deltaVelocityA;
    const real_t absFutureVelocityA = lengthVector( futureVelocityA );
    const real_t absDeltaVelocityA = absFutureVelocityA - absVelocityA;
    // # Скорость могла остаться прежней. Бережём память э. для др. событий.
    if ( !zero( absDeltaVelocityA ) ) {
        // @todo optimize? Ниже - повторение. Убрать его - ускорит обработку?
        // Общее событие об изменении скорости
        if (w < EMITTER_EVENT_COUNT) {
            const eventTwo_t e = {
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
        const eventTwo_t e = {
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
