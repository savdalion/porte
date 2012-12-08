// @require ���������� �� "include".


/**
* ��������� ����� � ������� �������.
*/



inline void exteriorCrustBiome(
    __global       biomeCell_t               bc,
    __global const surfaceTemperatureCell_t  stc,
    __global const rainfallCell_t            rc,
    __global const drainageCell_t            dc,
    __global const landscapeCell_t           lc,
    uint2*                                   rstate
) {
    /* @test
    bc[ 0 ].code = CB_TAIGA;
    bc[ 1 ].code = CB_TROPICAL_MOIST_BROADLEAF_FOREST;
    bc[ 2 ].code = CB_NONE;
    */

    // �� ����� ������ ������������� ���-�� ������, �.�. �. ����������� ��
    // ������ �������� ���������
    // # ��� �� �����, ���� ������ �������� �� ����� BIOME_CELL ������.

    // @see biome.hcl
    biome( bc, stc, rc, dc, lc, rstate );

}





__kernel void init(
    __global const aboutPlanet_t*             ap,    // 0
    __global biomeCell_t*                     bc,    // 1
    __global const temperatureCell_t*         tc,    // 2
    __global const surfaceTemperatureCell_t*  stc,   // 3
    __global const rainfallCell_t*            rc,    // 4
    __global const drainageCell_t*            dc,    // 5
    __global const landscapeCell_t*           lc,    // 6
    const uint                                seed   // 7
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

    // �������� � ��������� �����.
    const float distance = distanceNC( nc );

    // �������� ������ � ������������ �������
    // @todo ���������� ����� � � ������ ����� ����������� �������.
    if ( exteriorCrustZone( ap, distance, nc ) ) {
        exteriorCrustBiome( bc[i], stc[i], rc[i], dc[i], lc[i], &rstate );

    } else {
        // ��������� ������ ������ ��������
        for (uint k = 0; k < BIOME_CELL; ++k) {
            bc[i][k].code = CB_NONE;
        }
    }

}
