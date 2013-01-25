// @require ���������� �� "include".


/**
* ������ ������� �� �������� ������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� relative().
*
* #i NVIDIA 8800GTS �������� ������ � float.
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

    // # ���� �������� ������� ������. � �� �� ������ ��������� ���-��
    //   �������������� ���������.
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

    // ����� �� ���� ������ ������������ �� ���� ������
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


    // ���������, ����� �� ��� ������ ������������ � ���� ��������

    // ����������� ������� ��� ��������� ������
    //   # ����� ������ ���� ���� �� ���� ���������� ������� ������.
    const uint nStar = countStar( as, true );
    const bool hasNeedful = (nStar > 0);
    if ( !hasNeedful ) {
        // ��� ������� ��� �������� ������
#ifdef __DEBUG
        printf( "(?) Model %d / %d is out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
#endif
        return;
    }

    // # ������� ������ ������ � *����* �������� ������ ���� ������ ���������.
    const uint TRANSACT_SIZE = 4;
    if (w > (EMITTER_EVENT_COUNT - TRANSACT_SIZE)) {
        // � �������� ������������ ��������� ������, ����� ��������� ����
        // ������� �� 1 ������ ���� ������
        printf( "(?) Memory of asteroid %d is overfilled. Unique event is not commited.\n", element->uid );
        return;
    }


    // ����� �������������
    const real_t massA = massAsteroid( element );
    const real_t K_MASS = 1e-5f;
    const real_t deltaMassABySecond = massA * K_MASS;
    const real_t deltaMassAByTimestep = deltaMassABySecond * timestep;
    {
        const eventTwo_t e = {
            // uid �������
            E_CHANGE_MASS,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaMassABySecond, deltaMassAByTimestep }
        };
        ee->content[ w ] = e;
        ++w;
    }
    {
        const eventTwo_t e = {
            // uid �������
            E_INCREASE_MASS,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaMassABySecond, deltaMassAByTimestep }
        };
        ee->content[ w ] = e;
        ++w;
    }


    // ������ �������������
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
            // uid �������
            E_CHANGE_SIZE,
            // ������ �������� ������� - ����� �� �����
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
            // uid �������
            E_INCREASE_SIZE,
            // ������ �������� ������� - ����� �� �����
            {},
            {
                deltaSizeABySecond.s0,   deltaSizeABySecond.s1,   deltaSizeABySecond.s2,
                deltaSizeAByTimestep.s0, deltaSizeAByTimestep.s1, deltaSizeAByTimestep.s2
            }
        };
        ee->content[ w ] = e;
        ++w;
    }


    // @todo ��������� �����������, ������������� ��-�� ������� ����
    // @tip ����� �������� ������ �� �������, ����� ��������� �����������.


    // ��������� ������
    if (w != (ee->waldo + TRANSACT_SIZE)) {
        printf( "(!) Transact size for this model of asteroid %d is incorrect. %i %i\n",
            element->uid,  ee->waldo + TRANSACT_SIZE,  w );
    }
    ee->waldo = w;

}
