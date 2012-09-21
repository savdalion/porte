#include "include/pragma.hcl"
#include "include/restruct.hcl"
#include "include/helper.hcl"
#include "include/zone.hcl"
//#include "include/dice.hcl"


/**
* »нициализирует компоненты в области планеты.
*   #  омпоненты помещаютс€ на планету равномерно, сохран€€ заданные в
*     aboutPlanet_t пропорции.
*/
__kernel void init(
    __global aboutPlanet_t*    ap,    // 0
    __global componentCell_t*  cc     // 1
) {
    // всегда - координаты €чейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ќпцию -D при при построении €дра  
    const uint i = icDenorm( dnc );

}
