#include "../pragma.hcl"
#include "../struct.hcl"
#include "../helper.hcl"


/**
* ѕереносит температуру из временного объЄма (wb) в посто€нный (b).
*/
__kernel void fixResult(
    __global booster_t* b,    // 0
    __global booster_t* wb    // 1
) {
    // всегда - координаты €чейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );

    // @see ќпцию -D при при построении €дра
    const uint i = icDenorm( dnc );

    
    b->temperature[i] = wb->temperature[i];

}
