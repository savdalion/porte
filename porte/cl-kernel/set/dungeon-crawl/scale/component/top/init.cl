#include "include/pragma.hcl"
#include "include/restruct.hcl"
#include "include/helper.hcl"
#include "include/zone.hcl"
//#include "include/dice.hcl"


/**
* �������������� ���������� � ������� �������.
*   # ���������� ���������� �� ������� ����������, �������� �������� �
*     aboutPlanet_t ���������.
*/
__kernel void init(
    __global aboutPlanet_t*    ap,    // 0
    __global componentCell_t*  cc     // 1
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );

}
