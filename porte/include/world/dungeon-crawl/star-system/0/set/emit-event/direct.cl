// @require Библиотеки из "include".


/**
* Прямые события от элементов звёздной системы.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядро relative().
*
* #i NVIDIA 8800GTS работает только с float.
*/




inline void asteroidDirectEmitEvent(
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
        printf( "Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n", aai->uid, w );
    }
#endif


    // обязательные события для астероида
    // отсутствуют


    // Проверка на столкновения
    // # Проверяем все идущие ниже по алфавиту элементы.
    //   См. соглашение в теле EngineHybrid::pulse().
    const real4_t coordA = (real4_t)(
        convertFromBigValue( aai->today.coord.x ),
        convertFromBigValue( aai->today.coord.y ),
        convertFromBigValue( aai->today.coord.z ),
        0
    );
    const real_t maxSideA =
        max( max( aai->today.size[ 0 ],  aai->today.size[ 1 ] ),  aai->today.size[ 2 ] );
    const real_t massA = massAsteroid( aai );
    const real_t velocity1BeforeA = squareLengthVector( (real4_t)(
        aai->today.velocity[ 0 ],  aai->today.velocity[ 1 ],  aai->today.velocity[ 2 ],  0
    ) );
    const real_t kineticABefore = massA * velocity1BeforeA / 2.0f;

    // # Отсутствующий элемент - сигнал конца списка.
    // # Прекращаем запоминать события, если память переполнена.
    
    // Столкновения со звёздами
    //if (w < EMITTER_EVENT_COUNT) { - проверяется в цикле ниже.
    {
        // # Резервируем память, чтобы не переполнить её.
        const int MAX_EVENT_IN_ONE_LOOP = 2;
        for (uint k = 0;
            (k < STAR_COUNT) && presentStar( &as[ k ] ) && (w <= (EMITTER_EVENT_COUNT - MAX_EVENT_IN_ONE_LOOP));
            ++k
        ) {
            __global const aboutStar_t* ask = &as[ k ];

            const real4_t coordB = (real4_t)(
                ask->today.coord[ 0 ],
                ask->today.coord[ 1 ],
                ask->today.coord[ 2 ],
                0
            );
            const real_t collisionDistance =  max( maxSideA, ask->today.radius );
            const bool hasCollision = collision( coordA,  coordB,  collisionDistance );
            if ( hasCollision ) {
#ifdef __DEBUG
                printf( "direct() Asteroid %d collision with star %d.\n", aai->uid, ask->uid );
#endif
                // @todo optimize При столкновении астероида и звезды кинет.
                //       энергия не участвует в обработке события. Но есть
                //       слушатели...
                // силу удара определим по кинет. энергии
                const real_t massB = massStar( ask );
                const real_t velocity1BeforeB = squareLengthVector( (real4_t)(
                    ask->today.velocity[ 0 ],  ask->today.velocity[ 1 ],  ask->today.velocity[ 2 ],  0
                ) );
                const real_t kineticBBefore = massB * velocity1BeforeB / 2.0f;

                // @todo optimize Астероид слишком мал, чтобы существенно изменить
                //       скорость (и кинет. энергию) звезды. Можно не считать.
                real4_t velocityAAfter, velocityBAfter;
                speedCollisionVector(
                    &velocityAAfter,  &velocityBAfter,
                    massA,
                    (real4_t)( aai->today.velocity[ 0 ], aai->today.velocity[ 1 ], aai->today.velocity[ 2 ], 0 ),
                    massB,
                    (real4_t)( ask->today.velocity[ 0 ], ask->today.velocity[ 1 ], ask->today.velocity[ 2 ], 0 ),
                    0.9
                );
                const real_t velocity1AfterA = squareLengthVector( (real4_t)(
                    velocityAAfter[ 0 ],  velocityAAfter[ 1 ],  velocityAAfter[ 2 ],  0
                ) );
                const real_t velocity1AfterB = squareLengthVector( (real4_t)(
                    velocityBAfter[ 0 ],  velocityBAfter[ 1 ],  velocityBAfter[ 2 ],  0
                ) );
                const real_t kineticAAfter = massA * velocity1AfterA / 2.0f;
                const real_t kineticBAfter = massB * velocity1AfterB / 2.0f;
                const real_t deltaKineticA = kineticABefore - kineticAAfter;
                const real_t deltaKineticB = kineticBBefore - kineticBAfter;

                // при столкновении создаётся неск. событий

                // Столкновение со звездой
                {
                    eventTwo_t e = {
                        // uid события
                        E_COLLISION,
                        // второй участник события
                        { GE_STAR,  k,  ask->uid },
                        {
                            kineticABefore, kineticBBefore,
                            deltaKineticA,  deltaKineticB
                        }
                    };
                    aai->emitterEvent.content[ w ] = e;
                    ++w;
                }

                // Астероид уничтожен
                {
                    eventTwo_t e = {
                        // uid события
                        E_DESTROY,
                        // второй участник события - не важен для этого события
                        {},
                        {
                            kineticABefore,
                            deltaKineticA
                        }
                    };
                    aai->emitterEvent.content[ w ] = e;
                    ++w;
                }

            } // if ( hasCollision )

        } // for (size_t k = 0 ...

    } // Столкновения со звёздами


    // обновляем вальдо
    aai->emitterEvent.waldo = w;
}








inline void starDirectEmitEvent(
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
        printf( "Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n", asi->uid, w );
    }
#endif


    // обязательные события для звезды

    // Гравитация
    const real_t massA = massStar( asi );
    if (w < EMITTER_EVENT_COUNT) {
        // вычислим часть формулы для расчёта силы притяжения: это значение
        // смогут использовать элементы, которые притягивает звезда
        const real_t fgm = G * massA;
        eventTwo_t e = {
            // uid события
            E_GRAVITY,
            // второй участник события - здесь не важен
            {},
            { fgm }
        };
        asi->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Излучение энергии (ядерная реакция)
    // # Каждую секунду звезда излучает энергию.
    // @todo optimize Запоминать вычисленные значения.
    const real_t radiusA = asi->today.radius;
    const real_t surfaceTemperatureA = asi->today.surfaceTemperature;
    const real_t luminosityBySecond =
        luminosity( radiusA, surfaceTemperatureA );
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_RADIATION,
            // второй участник события - здесь не важен
            {},
            { luminosityBySecond, luminosityBySecond * timestep }
        };
        asi->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Потеря массы (из-за излучения)
    // @todo optimize Запоминать вычисленные значения.
    // E = m * c ^ 2
    const real_t deltaMassBySecond =
        -luminosityBySecond / (SPEED_LIGHT * SPEED_LIGHT);
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_CHANGE_MASS,
            // второй участник события - здесь не важен
            {},
            { deltaMassBySecond, deltaMassBySecond * timestep }
        };
        asi->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Проверка на столкновения
    // # Проверка на столкновения осуществляется идущими выше по алфавиту
    //   элементами. См. соглашение в теле EngineHybrid::pulse().


    // обновляем вальдо
    asi->emitterEvent.waldo = w;
}




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void direct(
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
        asteroidDirectEmitEvent( i, aa, ap, as, timestep );
    }

    if ( (i < PLANET_COUNT) && presentPlanet( &ap[ i ] ) ) {
        //planetDirectEmitEvent( pc[ i ] );
    }

    if ( (i < STAR_COUNT) && presentStar( &as[ i ] ) ) {
        starDirectEmitEvent( i, aa, ap, as, timestep );
    }

}
