// @require ���������� �� "include".


/**
* ������ ��� ������ � ������.
*
* @see ���������� � helper.hcl.
*/



/**
* ��������� ��������� "bc", ��������� BIOM_CELL ������ �� ��������
* ����������.
*/
inline void biome(
    __global       biomeCell_t               bc,
    __global const temperatureCell_t         tc,
    __global const surfaceTemperatureCell_t  stc,
    __global const rainfallCell_t            rc,
    __global const drainageCell_t            dc,
    __global const landscapeCell_t           lc,
    const uint                               seed
) {
    // @test
    bc[ 0 ].code = CB_TAIGA;
    bc[ 1 ].code = CB_TROPICAL_MOIST_BROADLEAF_FOREST;
    bc[ 2 ].code = CB_NONE;

    // @todo ...
}
