// @require Библиотеки из "include".


/**
* Методы для работы с ландшафтом и его элементами.
*
* @see Соглашения в helper.hcl.
*/



/**
* Заполняет структуру "lc", определяя случайный эл. ландшафта по заданным
* параметрам.
*/
inline void landscape(
    __global       elementLandscape_t*       el,
    __global const componentCell_t           cc,
    __global const temperatureCell_t         tc,
    __global const surfaceTemperatureCell_t  stc,
    __global const rainfallCell_t            rc,
    __global const drainageCell_t            dc,
    const uint2*                             rstate
) {
    // @test
    el->code = CEL_FLAT;
    el->count = 1.0f;
    el->scattering = 2.0f;

    // @todo ...
}
