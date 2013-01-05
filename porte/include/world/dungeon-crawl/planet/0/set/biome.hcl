// @require ���������� �� "include".


/**
* ������ ��� ������ � ������.
*
* @see ���������� � helper.hcl.
*/


// @define biome-utils.h
// @todo fine �����������, ������ ������ �������� ����� ������ � biome-utils.h.
void likeBiome(
    __global presentBiome_t* bc,
    const float temperature,
    const float rainfall,
    const float drainage,
    __global const elementLandscape_t* landscapeCell,
    const size_t randomValue
);




/**
* ��������� ��������� "bc", ������� ����� �� �������� ����������.
*/
inline void biome(
    __global       presentBiome_t*                 bc,
    __global const behaviourSurfaceTemperature_t*  stc,
    __global const behaviourRainfall_t*            rc,
    __global const behaviourDrainage_t*            dc,
    __global const elementLandscape_t*             lc,
    uint2*                                         rstate
) {
    // @see biome-utils.h
    const float temperature = stc[0].average;
    const float rainfall = rc[0].average;
    const float drainage = dc[0].average;
    const uint randomValue = uintRandom( rstate );
    likeBiome(
        bc,
        temperature,
        rainfall,
        drainage,
        lc,
        randomValue
    );
}
