// @require Библиотеки из "include".


/**
* Прямые события от элементов звёздной системы.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядро relative().
*
* #i NVIDIA 8800GTS работает только с float.
*/




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
#endif

    int w = ee->waldo;


    // Обязательные события
    // отсутствуют


    // Проверка на столкновения
    // # Проверяем все идущие ниже по алфавиту элементы.
    //   См. соглашение в теле EngineHybrid::pulse().
    const real4_t coordA = convertFromBig3DValue( element->today.coord );
    //const real_t maxSideA =
    //    fmax( fmax( element->today.size.x,  element->today.size.y ), element->today.size.z );
    real_t maxSideA = fmax( element->today.size.s0,  element->today.size.s1 );
    //maxSideA = 1000;
    @todo ! Не хватает регистров?

    const real_t massA = massAsteroid( element );
    const real_t velocity1BeforeA =
        squareLengthVector( element->today.velocity );
    const real_t kineticABefore =
        massA * velocity1BeforeA / 2.0f;

    // # Отсутствующий элемент - сигнал конца списка.
    // # Прекращаем запоминать события, если память переполнена.
    
    // Столкновения со звёздами
    //if (w < EMITTER_EVENT_COUNT) { - проверяется в цикле ниже.
    {
        // # Резервируем память, чтобы не переполнить её.
        const int MAX_EVENT_IN_ONE_LOOP = 2;
        for (uint k = 0;
            // # Здесь можем воспользоваться признаком "Пустой элемент -
            //   дальше нет событий", т.к. элементы в начале каждого
            //   пульса - упорядочены. См. соглашение в начале пульса.
            (k < STAR_COUNT) && presentStar( &as[ k ] )
         && (w <= (EMITTER_EVENT_COUNT - MAX_EVENT_IN_ONE_LOOP));
            ++k
        ) {
            __global const aboutStar_t* ask = &as[ k ];

            const real4_t coordB = convertFromBig3DValue( ask->today.coord );
            const real_t collisionDistance = fmax( maxSideA, ask->today.radius );
            const bool hasCollision = collision( coordA,  coordB,  collisionDistance );
            if ( !hasCollision ) {
                continue;
            }

#ifdef __DEBUG
            printf( "direct() Asteroid %d collision with star %d.\n", element->uid, ask->uid );
#endif
            // @todo optimize При столкновении астероида и звезды кинет.
            //       энергия не участвует в обработке события. Но есть
            //       слушатели...
            // силу удара определим по кинет. энергии
            const real_t massB = massStar( ask );
            const real_t velocity1BeforeB =
                squareLengthVector( ask->today.velocity );
            const real_t kineticBBefore =
                massB * velocity1BeforeB / 2.0f;

            // @todo optimize Астероид слишком мал, чтобы существенно изменить
            //       скорость (и кинет. энергию) звезды. Можно не считать.
            real4_t velocityAAfter, velocityBAfter;
            speedCollisionVector(
                &velocityAAfter,  &velocityBAfter,
                massA,
                element->today.velocity,
                massB,
                ask->today.velocity,
                0.9
            );
            // @todo optimize fine Использовать 4-е поле для вычисления и
            //       хранения длины вектора.
            const real_t velocity1AfterA =
                squareLengthVector( velocityAAfter );
            const real_t velocity1AfterB =
                squareLengthVector( velocityBAfter );
            const real_t kineticAAfter = massA * velocity1AfterA / 2.0f;
            const real_t kineticBAfter = massB * velocity1AfterB / 2.0f;
            const real_t deltaKineticA = kineticABefore - kineticAAfter;
            const real_t deltaKineticB = kineticBBefore - kineticBAfter;

            // при столкновении создаётся неск. событий
            // #! Добавление сюда новых событий требует обновления
            //    'MAX_EVENT_IN_ONE_LOOP' выше.

            // Столкновение со звездой
            {
                const eventTwo_t e = {
                    // uid события
                    E_COLLISION,
                    // второй участник события
                    { GE_STAR,  k,  ask->uid },
                    {
                        kineticABefore, kineticBBefore,
                        deltaKineticA,  deltaKineticB
                    }
                };
                //element->emitterEvent.content[ w ] = e;
                //++w;
            }

#if 0
            // Астероид уничтожен
            {
                const eventTwo_t e = {
                    // uid события
                    E_DESTROY,
                    // второй участник события - не важен для этого события
                    {},
                    {
                        kineticABefore,
                        deltaKineticA
                    }
                };
                element->emitterEvent.content[ w ] = e;
                ++w;
            }

            // #! Добавление сюда новых событий требует обновления
            //    'MAX_EVENT_IN_ONE_LOOP' выше.
#endif

        } // for (size_t k = 0 ...

    } // Столкновения со звёздами


    // обновляем вальдо
    //ee->waldo = w;

}
