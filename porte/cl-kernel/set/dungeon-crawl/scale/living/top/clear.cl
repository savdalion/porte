#include "include/pragma.hcl"
#include "include/restruct.hcl"
#include "include/helper.hcl"
#include "include/zone.hcl"
#include "include/dice.hcl"


/**
* ������� ���������� �� ������ � ������� �������.
*/
__kernel void clear(
    __global livingCell_t*  lc    // 0
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );

    
    // ��������� �������� ����� � ����������� �� ������
    for (uint o = 0; o < LIVING_CELL; ++o) {
        for (uint l = 0; l < LIFE_CYCLE_COUNT; ++l) {
            lc[i][o][l].code  = CL_NONE;
            lc[i][o][l].count = 0.0f;
        }
    }

}
