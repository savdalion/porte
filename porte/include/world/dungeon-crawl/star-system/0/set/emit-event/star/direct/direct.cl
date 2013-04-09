#include "pragma.hcl"
#include "restruct.hcl"
#include "helper.hcl"


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
    __global const todayStar_t*   ts,       // 0
    __global emitterEventStar_t*  ees,      // 1
    const real_t                  timestep  // 4
) {
    //  ������ ��������
    const uint i = get_global_id( 0 );
    // @todo optimize ����� �� ���������.
    if (i >= STAR_COUNT) {
        return;
    }

    __global characteristicStar_t* element = &ts[ i ];
    if ( absentStar( element ) ) {
        return;
    }

    ������ ����������� ������ �������?
    ��� ������ ������� ����� �����, � ����� ������ ������ �����������?
    
    //  ������ ����������� ������ ������� ��������
    const uint im = get_global_id( 1 );
    // @todo optimize ����� �� ���������.
    if (im >= EMIT_EVENT_STAR_COUNT) {
        return;
    }


    __global eventTwo_t* e = &ee[ i ][ im ];



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
    const real_t luminosityABySecond =
        luminosity( radiusA, surfaceTemperatureA );
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
