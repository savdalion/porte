#include "include/pragma.hcl"
#include "include/restruct.hcl"
#include "include/helper.hcl"
#include "include/zone.hcl"
#include "include/dice.hcl"


/**
* ќчищает информацию об особ€х в области планеты.
*/
__kernel void clear(
    __global livingCell_t*  lc    // 0
) {
    // всегда - координаты €чейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ќпцию -D при при построении €дра  
    const uint i = icDenorm( dnc );

    
    // заполн€ем пустотой места с информацией об особ€х
    for (uint o = 0; o < LIVING_CELL; ++o) {
        for (uint l = 0; l < LIFE_CYCLE_COUNT; ++l) {
            lc[i][o][l].code  = CL_NONE;
            lc[i][o][l].count = 0.0f;
        }
    }

}
