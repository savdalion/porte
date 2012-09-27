#include "include/pragma.hcl"
#include "include/restruct.hcl"
#include "include/helper.hcl"
#include "include/zone.hcl"
#include "include/dice.hcl"


/**
* �������������� ������ � ������� �������.
*
* #! ����� ������� ����� ���� ������� ��������� ���� "clear".
*/



#ifdef WITH_NEIGHBOUR_EXPANSION
/**
* @return ���-�� �������� ������.
*
* @param code ���� ��� = CL_NONE, ��������� ���-�� ����� ������.
*/
inline float neighbourCount(
    const enum CODE_LIVING code,
    const enum LIFE_CYCLE lifeCycle,
    __global livingCell_t* lc,
    const int4 nc
) {
    float count = 0.0f;

    // ��������� �������������� � ������������ ������,
    // ��������� �� �������
    const static uint side[6] = { 0, 1, 2, 3, 9, 18 };
    for (uint q = 0; q < 6; ++q) {
        const int4 neighbourNC = isc( side[q] );
        const uint ni = ic( neighbourNC );
        for (uint o = 0; o < LIVING_CELL; ++o) {
            if ( (code == CL_NONE) || (code == lc[ ni ][ o ][ lifeCycle ].code) ) {
                count += lc[ ni ][ o ][ lifeCycle ].count;
            }
        }
    }

    return count;
}
#endif




/**
* �������������� ������ � �������� ���� ������� �������.
*   # ��������, ��� ���������� ��������� � ��������� ���� ��� ������������.
*/
inline void zoneLiving(
    __global livingCell_t* lc,
    __global const zoneLiving_t* z,
    __global const zoneLivingCountComplete_t* zcc,
    const enum LIFE_CYCLE lifeCycle,
    uint2* rstate,
    const int4 nc,
    const uint i
) {
    // ���-�� ������ � ������� ������� ����������� �����������.

    // �������� �� ���� ��������� � ���� ���� ������� ������
    // # ���� ������ �������� �� ����� LIVING_CELL ������.
    for (uint k = 0; k < LIVING_CELL; ++k) {
        // lc[i][k][LC] typeof portionLiving_t
        // # ����� ���� �������� �� ����� LIVING_CELL ������.
        const enum CODE_LIVING code = z[k].code;
        if (code == CL_NONE) {
            // ������ �����, ������ �� ���������
            continue;
        }
        
        // ������������, ����� ����� ���-�� ������ �� ��������� ��������.
        // @todo extend? ����� �������� �� �����?
        const bool countComplete = zcc[code][lifeCycle]->all;
        if ( countComplete ) {
            continue;
        }
        
        // ��������� � ���������� ����� ��������
        rstate->x ^= (uint)code ^ (uint)lifeCycle ^ k;

        // ���� ���������� ���-�� ������ ������������ ������������.
        // ����������� ��������� ����� ����� ������ � ���� ������
        uint diceGroup = dice( rstate, 1, 20 );

        // ���������, ���� �� ��� ���� ��� ����� � ���� ������
        uint o = 0;
        for (; o < LIVING_CELL; ++o) {
            if (lc[i][o][lifeCycle].code == code) {
                // ����� ��� ����
                // ��� ����� ������������ ���������
                rstate->y ^= (uint)( lc[i][o][lifeCycle].count );
                break;
            }
        }
        const bool absent = (o >= LIVING_CELL);


        // ������ "����������� �����": �� �������� ������� ���-�� �� ��������.
        //float grow = floatDice( rstate, 1, 20.0 );
        float grow = floatDiapasonRandom( rstate,  (float2)( -0.2f, 1.0f ) );


#ifdef WITH_SELF_EXPANSION
        // ������� � ������� ������ �������� ������ �������� �����������
        // ���������� ���-�� ������ � ���� �� ������.
        if ( !absent ) {
            diceGroup++;
            diceGroup++;
            grow *= 2.0f;
        }
#endif

#ifdef WITH_NEIGHBOUR_EXPANSION
        // ������� � �������� ������� �������� ������ �������� �����������
        // ���������� ���-�� ������ � ������� ������.
        // ������� �� �������
        // #! �.�. ���������� ������ ������ ��� ������������� �������
        //    � � ���� OpenCL ����������� ����������� �������������� ������
        //    �� ���������� ������, �������� ���������� ��� �������� ���-��
        //    �������. ����� ������ � ���-�� - ������ ���������. �� ����
        //    �������, ��� � ����� ������ ��� ������ ������ ����������
        //    ������ ���� ��� ���������� "seed" (��. __kernel).
        const float n = neighbourCount( code, lifeCycle, lc, nc );
        if (n > 0.0f) {
            diceGroup++;
            grow *= 1.3f;
        }
        if (n > 1000000.0f) {
            diceGroup++;
            grow *= 1.3f;
        }
        if (n > 1000000000.0f) {
            diceGroup++;
            grow *= 1.3f;
        }
#endif

        if (diceGroup < 15) {
            // ����� ������ ���� ������ - �������. �� "code" - �� ����������
            continue;
        }
        
        // ��������� ����� ������ ������ "code"
        if ( absent ) {
            // ������������� ��������� �� ������ ��������� ������
            o = 0;
            for ( ; o < LIVING_CELL; ++o) {
                if (lc[i][o][lifeCycle].code == CL_NONE) {
                    // ������ � ���. �� ����� ��������
                    break;
                }
            }            
        }
        if (o >= LIVING_CELL) {
            // ��� ����� ������, ����� ����� �� ����� ���� ���������
            // � ��� ������
            // @todo optimize bad ������ ��������. ���� # ���-�� ����������
            //       � ���������� ������ ������� �� ��������� ����. ���-��
            //       ���� ��� ������ � ������.
            return;
        }
        // ����� "o" �������������� ��������� �� ��������� ����� ��� �����

        // �.�. ����� ������������� "����������" ����� ���� ������,
        // �������� ���� �������, �������� ������� ���-��.
        /* - �� ����������.
        const float minGroup = z[k].minGroup;
        const float maxGroup = z[k].maxGroup;
        const float averageGroup = (minGroup + maxGroup) / 2.0f;
        */

        /* - ������ ������. ��. ����.
        // ��������� ����������� � ������ ���-�� ����� � ������� ������� �
        // �������, ��� ���������� ���������� ����������
        static const float VOLUME_K = (float)(GRID * GRID * GRID / 2);

        const float delta =
            z[k].count / averageGroup * (float)GROW_SPEED / VOLUME_K;

        lc[i][o][lifeCycle].code = code;
        const float qty = (float)intDiapasonRandom(
            rstate,  (int2)( minGroup, maxGroup )
        ) * delta;
        lc[i][o][lifeCycle].count += qty;
        */
        
        lc[i][o][lifeCycle].code = code;
        static const float VOLUME_K = (float)(GRID * GRID * GRID / 9 + 1);
        const float powerIteration = z[k].count / ((float)ITERATION_GROW_COUNT * VOLUME_K);
        const float qty = grow * floatDice( rstate, 1, powerIteration );
        lc[i][o][lifeCycle].count += qty;
        // @todo optimize ����������� ����� ��������� ����� ���� init-��������.
        if (lc[i][o][lifeCycle].count < 1.0f) {
            lc[i][o][lifeCycle].count = floatDice( rstate, 1, powerIteration ) + 1.0f;
        }

    } // for (uint k

}





__kernel void init(
    __global const aboutPlanet_t*               ap,    // 0
    __global livingCell_t*                      lc,    // 1
    // # �� ���������� ����������� �� ����� ��� �������� ���-��.
    __global const zoneLivingCountComplete_t*  zcc,    // 2
    const enum LIFE_CYCLE                lifeCycle,    // 3
    __global const componentCell_t*             cc,    // 4
    __global const temperatureCell_t*           tc,    // 5
    const uint                                seed     // 6
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );


    // �������������� ��������� ����. ����� ��� ���� ������
    uint2 rstate = (uint2)( seed, i );


    // ���������� ��������� �� ����� ������� �������.
    // �������� � ��������� ���������.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    // ��������� ����
    if ( coreZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.core, zcc, lifeCycle, &rstate, nc, i );

    } else if ( mantleZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.mantle, zcc, lifeCycle, &rstate, nc, i );

    } else if ( crustZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.crust, zcc, lifeCycle, &rstate, nc, i );

    } else if ( atmosphereZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.atmosphere, zcc, lifeCycle, &rstate, nc, i );

    } else {
        zoneLiving( lc, ap->living.space, zcc, lifeCycle, &rstate, nc, i );
    }

}
