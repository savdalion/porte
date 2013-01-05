// @require ���������� �� "include".


/**
* ���� "A" ������������� ����� ��������� � ������� �������.
*/


inline void exteriorCrustLandscape(
    __global       elementLandscape_t*             lc,
    __global const landscapePlanet_t*              lp,
    __global const portionComponent_t*             cc,
    __global const behaviourTemperature_t*         tc,
    __global const behaviourSurfaceTemperature_t*  stc,
    __global const behaviourRainfall_t*            rc,
    __global const behaviourDrainage_t*            dc,
    uint2*                                         rstate
) {
    // ����� ���-�� ��������� ��������� � ���� ������
    // # ���� ������ �������� �� ����� LANDSCAPE_CELL ����������� ����������.
    const uint count = dice( rstate, 1, LANDSCAPE_CELL + 1 ) - 1;

    uint k = 0;
    for ( ; k < count; ++k) {
        // @see landscape.hcl
        landscape( &lc[k], lp, cc, tc, stc, rc, dc, rstate );
    }

    // ����������� ��������
    for ( ; k < LANDSCAPE_CELL; ++k) {
        lc[k].code  = CEL_NONE;
        // @todo optimize ���� ���� ����� �� ����������������.
        const aboutElementLandscape_t about = {};
        lc[k].about = about;
        lc[k].count = 0.0f;
        lc[k].scattering = 0.0f;
    }

}





__kernel void initA(
    __global const aboutPlanet_t*             ap,    // 0
    __global landscapeCell_t*                 lc,    // 1
    __global const componentCell_t*           cc,    // 2
    __global const temperatureCell_t*         tc,    // 3
    __global const surfaceTemperatureCell_t*  stc,   // 4
    __global const rainfallCell_t*            rc,    // 5
    __global const drainageCell_t*            dc,    // 6
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
    // # �������� (�������� ���������), ���������� �� ������ �� �����������
    //   �������, �� � ������ ��.
    if ( exteriorCrustZone( ap, distance, nc ) ) {
        exteriorCrustLandscape(
            lc[i],
            &ap->landscape,
            cc[i],
            tc[i],
            stc[i],
            rc[i],
            dc[i],
            &rstate
        );

    } else if ( true ) {
        // @todo �������� �������� ������ ������� (������).

    } else {
        // �.�. ��� ������������� ���������� ����� ���, ��������� ������ ���
        // �������� - ������������. ��������� �������� � ���� clear().
    }

}
