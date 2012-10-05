// @require ���������� �� "include".


/**
* ������� ���������� � ��������� � ������� �������.
*/
__kernel void clear(
    __global landscapeCell_t*  lc    // 0
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );

    
    // ��������� �������� ����� � ����������� � ���������
    for (uint o = 0; o < LANDSCAPE_CELL; ++o) {
        lc[i][o].code  = CEL_NONE;
        const aboutElementLandscape_t about = {};
        lc[i][o].about = about;
        lc[i][o].count = 0.0f;
        lc[i][o].scattering = 0.0f;
    }

}
