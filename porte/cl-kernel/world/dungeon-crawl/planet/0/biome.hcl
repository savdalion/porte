// @require Библиотеки из "include".


/**
* Методы для работы с биомом.
*
* @see Соглашения в helper.hcl.
*/


// @define biome-utils.h
// @todo fine Разобраться, почему нельзя объявить метод только в biome-utils.h.
void likeBiome(
    __global biomeCell_t bc,
    const float temperature,
    const float rainfall,
    const float drainage,
    __global const landscapeCell_t landscapeCell,
    const size_t randomValue
);



/**
* Заполняет структуру "bc", выявляя биомы по заданным параметрам.
*/
inline void biome(
    __global       biomeCell_t               bc,
    __global const surfaceTemperatureCell_t  stc,
    __global const rainfallCell_t            rc,
    __global const drainageCell_t            dc,
    __global const landscapeCell_t           lc,
    uint2*                                   rstate
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
