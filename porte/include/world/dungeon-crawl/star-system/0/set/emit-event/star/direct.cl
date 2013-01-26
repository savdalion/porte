// @require ���������� �� "include".


/**
* ������ ������� �� ��������� ������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� relative().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void direct(
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    // @todo __global real4_t                   di
    const real_t                       timestep  // 4
) {
    return;

    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );

    if (i >= STAR_COUNT) {
        //printf( "(!) Index %d / %d out of range for star.\n",  i,  STAR_COUNT - 1 );
        // @todo di.s0 = CLP_INDEX_OUT_OF_RANGE;
        return;
    }

    if ( absentStar( &as[ i ] ) ) {
        return;
    }


    __global aboutStar_t* element = &as[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
    __global eventTwo_t* eec = element->emitterEvent.content;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        //printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
        //    element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


    // ������������ �������

    // ����������
    if (w < EMITTER_EVENT_COUNT) {
        // �������� ����� ������� ��� ������� ���� ����������: ��� ��������
        // ������ ������������ ��������, ������� ����������� ������
        const real_t fgm = G * element->today.mass;
        const eventTwo_t e = {
            // uid �������
            E_GRAVITY,
            // ������ �������� ������� - ����� �� �����
            {},
            { fgm }
        };
        //eec[ w ] = e;
        eec[ w ].uid = E_GRAVITY;
        eec[ w ].fReal[ 0 ] = fgm;
        ++w;
    }


    // ��������� ������� (������� �������)
    // # ������ ������� ������ �������� �������.
    // @todo optimize ���������� ����������� ��������.
    const real_t radiusA = element->today.radius;
    const real_t surfaceTemperatureA = element->today.surfaceTemperature;
    const real_t luminosityABySecond = 0;
        //luminosity( radiusA, surfaceTemperatureA );
    if (w < EMITTER_EVENT_COUNT) {
        const eventTwo_t e = {
            // uid �������
            E_RADIATION,
            // ������ �������� ������� - ����� �� �����
            {},
            { luminosityABySecond, luminosityABySecond * timestep }
        };
        eec[ w ] = e;
        //eec[ w ].uid = E_RADIATION;
        //eec[ w ].fReal[ 0 ] = luminosityABySecond;
        //eec[ w ].fReal[ 1 ] = luminosityABySecond * timestep;
        ++w;
    }


    // ������ ����� (��-�� ���������)
    // @todo optimize ���������� ����������� ��������.
    // E = m * c ^ 2
    const real_t deltaMassABySecond =
        -luminosityABySecond / (SPEED_LIGHT * SPEED_LIGHT);
    if (w < EMITTER_EVENT_COUNT) {
        const eventTwo_t e = {
            // uid �������
            E_CHANGE_MASS,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaMassABySecond, deltaMassABySecond * timestep }
        };
        eec[ w ] = e;
        ++w;
    }


    // �������� �� ������������
    // # �������� �� ������������ �������������� ������� ���� �� ��������
    //   ����������. ��. ���������� � ���� EngineHybrid::pulse().


    // ��������� ������
    ee->waldo = w;

}
