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
    const real_t                       timestep  // 4
) {
    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );

    if ( (i > STAR_COUNT) || absentStar( &as[ i ] ) ) {
        return;
    }


    __global aboutStar_t* element = &as[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


    // ������������ �������

    // ����������
    const real_t massA = massStar( element );
    if (w < EMITTER_EVENT_COUNT) {
        // �������� ����� ������� ��� ������� ���� ����������: ��� ��������
        // ������ ������������ ��������, ������� ����������� ������
        const real_t fgm = G * massA;
        eventTwo_t e = {
            // uid �������
            E_GRAVITY,
            // ������ �������� ������� - ����� �� �����
            {},
            { fgm }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ������� (������� �������)
    // # ������ ������� ������ �������� �������.
    // @todo optimize ���������� ����������� ��������.
    const real_t radiusA = element->today.radius;
    const real_t surfaceTemperatureA = element->today.surfaceTemperature;
    const real_t luminosityBySecond =
        luminosity( radiusA, surfaceTemperatureA );
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid �������
            E_RADIATION,
            // ������ �������� ������� - ����� �� �����
            {},
            { luminosityBySecond, luminosityBySecond * timestep }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ������ ����� (��-�� ���������)
    // @todo optimize ���������� ����������� ��������.
    // E = m * c ^ 2
    const real_t deltaMassBySecond =
        -luminosityBySecond / (SPEED_LIGHT * SPEED_LIGHT);
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid �������
            E_CHANGE_MASS,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaMassBySecond, deltaMassBySecond * timestep }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid �������
            E_DECREASE_MASS,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaMassBySecond, deltaMassBySecond * timestep }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // �������� �� ������������
    // # �������� �� ������������ �������������� ������� ���� �� ��������
    //   ����������. ��. ���������� � ���� EngineHybrid::pulse().


    // ��������� ������
    ee->waldo = w;

}
