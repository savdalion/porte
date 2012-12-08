#include "../pragma.hcl"
#include "../struct.hcl"
#include "../helper.hcl"


/**
* ��������� ����������� �� ���������� ������ (wb) � ���������� (b).
*/
__kernel void fixResult(
    __global booster_t* b,    // 0
    __global booster_t* wb    // 1
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );

    // @see ����� -D ��� ��� ���������� ����
    const uint i = icDenorm( dnc );

    
    b->temperature[i] = wb->temperature[i];

}
