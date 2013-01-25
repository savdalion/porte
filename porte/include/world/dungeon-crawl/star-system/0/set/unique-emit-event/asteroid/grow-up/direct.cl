// @require Ѕиблиотеки из "include".


/**
* ѕр€мые событи€ от элемента звЄздной системы.
*
* @see  оммент. в starsystem::l0::EngineHybrid::pulse().
* @see ядро relative().
*
* #i NVIDIA 8800GTS работает только с float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void direct(
    __global const aboutStarSystem_t*  ass,       // 0
    __global aboutAsteroid_t*          aa,        // 1
    __global aboutPlanet_t*            ap,        // 2
    __global aboutStar_t*              as,        // 3
    const real_t                       timestep,  // 4
    const long                         pulselive  // 5
) {
    return;

    // # —юда получаем готовый индекс. » он не должен превышать кол-во
    //   обрабатываемых элементов.
    const uint i = get_global_id( 0 );
#ifdef __DEBUG
    /* @test
    printf( "get_global_size() %d\n",  get_global_size( 0 ) );
    printf( "get_local_size() %d\n",   get_local_size( 0 ) );
    printf( "get_global_id() %d\n",    get_global_id( 0 ) );
    printf( "get_group_id() %d\n",     get_group_id( 0 ) );
    */
    if (i >= ASTEROID_COUNT) {
        printf( "(!) Index %d / %d is out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
        return;
    }
#endif

    if ( absentAsteroid( &aa[ i ] ) ) {
        return;
    }

    // нужно ли этой модели пульсировать на этом пульсе
    const char model[] = "grow-up";
    if ( !needPulseModelAsteroid( &aa[ i ], model, pulselive ) ) {
        //printf( "Skip model %s for asteroid %d on pulse %i.\n",  model,  aa[ i ].uid,  pulselive );
        return;
    }


    __global aboutAsteroid_t* element = &aa[ i ];
    __global frequencyMemoryModel_t* fmm = &element->frequencyMemoryModel;
#ifdef __DEBUG
    if ( !betweenInteger( fmm->waldo, 0, FREQUENCY_MEMORY_MODEL_COUNT - 1 ) ) {
        printf( "(?) Frequency memory model of asteroid %d is overfilled. Waldo = %i / %d.\n",
            element->uid,  fmm->waldo,  FREQUENCY_MEMORY_MODEL_COUNT - 1 );
    }
#endif

    __global emitterEvent_t* ee = &element->emitterEvent;
    int w = ee->waldo;


    // провер€ем, может ли эта модель пульсировать в этом элементе

    // Ќеобходимые услови€ дл€ пульсации модели
    //   # –€дом должна быть хот€ бы одна излучающа€ энергию звезда.
    const uint nStar = countStar( as, true );
    const bool hasNeedful = (nStar > 0);
    if ( !hasNeedful ) {
        // нет условий дл€ развити€ модели
#ifdef __DEBUG
        printf( "(?) Model %d / %d is out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
#endif
        return;
    }

    // # Ёлемент должен узнать о *всех* событи€х пульса этой модели поведени€.
    const uint TRANSACT_SIZE = 4;
    if (w > (EMITTER_EVENT_COUNT - TRANSACT_SIZE)) {
        // у элемента недостаточно свободной пам€ти, чтобы запомнить блок
        // событий от 1 пульса этой модели
        printf( "(?) Memory of asteroid %d is overfilled. Unique event is not commited.\n", element->uid );
        return;
    }


    // ћасса увеличиваетс€
    const real_t massA = massAsteroid( element );
    const real_t K_MASS = 1e-5f;
    const real_t deltaMassABySecond = massA * K_MASS;
    const real_t deltaMassAByTimestep = deltaMassABySecond * timestep;
    {
        const eventTwo_t e = {
            // uid событи€
            E_CHANGE_MASS,
            // второй участник событи€ - здесь не важен
            {},
            { deltaMassABySecond, deltaMassAByTimestep }
        };
        ee->content[ w ] = e;
        ++w;
    }
    {
        const eventTwo_t e = {
            // uid событи€
            E_INCREASE_MASS,
            // второй участник событи€ - здесь не важен
            {},
            { deltaMassABySecond, deltaMassAByTimestep }
        };
        ee->content[ w ] = e;
        ++w;
    }


    // –азмер увеличиваетс€
    const real4_t sizeA = (real4_t)(
        element->today.size[ 0 ],
        element->today.size[ 1 ],
        element->today.size[ 2 ],
        0
    );
    const real_t K_SIZE = K_MASS / 10.0f;
    const real4_t deltaSizeABySecond = sizeA * K_SIZE;
    const real4_t deltaSizeAByTimestep = deltaSizeABySecond * timestep;
    {
        const eventTwo_t e = {
            // uid событи€
            E_CHANGE_SIZE,
            // второй участник событи€ - здесь не важен
            {},
            {
                deltaSizeABySecond.s0,   deltaSizeABySecond.s1,   deltaSizeABySecond.s2,
                deltaSizeAByTimestep.s0, deltaSizeAByTimestep.s1, deltaSizeAByTimestep.s2
            }
        };
        ee->content[ w ] = e;
        ++w;
    }
    {
        const eventTwo_t e = {
            // uid событи€
            E_INCREASE_SIZE,
            // второй участник событи€ - здесь не важен
            {},
            {
                deltaSizeABySecond.s0,   deltaSizeABySecond.s1,   deltaSizeABySecond.s2,
                deltaSizeAByTimestep.s0, deltaSizeAByTimestep.s1, deltaSizeAByTimestep.s2
            }
        };
        ee->content[ w ] = e;
        ++w;
    }


    // @todo ѕлотность сохран€етс€, пересчитываем из-за событий выше
    // @tip ћожно измен€ть размер на столько, чтобы плотность сохран€лась.


    // обновл€ем вальдо
    if (w != (ee->waldo + TRANSACT_SIZE)) {
        printf( "(!) Transact size for this model of asteroid %d is incorrect. %i %i\n",
            element->uid,  ee->waldo + TRANSACT_SIZE,  w );
    }
    ee->waldo = w;

}
