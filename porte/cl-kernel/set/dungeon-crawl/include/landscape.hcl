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
    __global const landscapePlanet_t*        lp,
    __global const componentCell_t           cc,
    __global const temperatureCell_t         tc,
    __global const surfaceTemperatureCell_t  stc,
    __global const rainfallCell_t            rc,
    __global const drainageCell_t            dc,
    uint2*                                   rstate
) {
    /* @test
    el->code = CEL_FLAT;
    el->count = 1.0f;
    el->scattering = 2.0f;
    */

    const enum CODE_ELEMENT_LANDSCAPE code =
        (enum CODE_ELEMENT_LANDSCAPE)intDiapasonRandom( rstate,  (int2)( 1, CEL_last - 1 ) );

    // @todo optimize Без инициализации.
    aboutElementLandscape_t about = {};

    switch ( code ) {
        case CEL_FLAT:
            // нет параметров
            break;

        case CEL_MOUNTAIN:
            about.mountain.height =
                floatDeviationRandom( rstate, lp->mountain.height, lp->deviationMountain );
            about.mountain.slope =
                floatDeviationRandom( rstate, lp->mountain.slope,  lp->deviationMountain );
            break;

        case CEL_BASIN:
            break;

        case CEL_RANGE:
            break;

        case CEL_RAVINE:
            break;

        case CEL_LAKE:
            break;

        case CEL_RIVER:
            break;

        case CEL_CASCADE:
            break;

        case CEL_ROAD:
            break;

        case CEL_CAVE:
            break;

        default:
            // без инициализации
            break;
    }

    el->code = code;
    el->about = about;
    el->count = dice( rstate, 1, 100 );
    el->scattering = floatDice( rstate, 1, 1.0f );
    // @todo Добавить частичное размещение элемента ландшафта в ячейке.
    el->partially = D_NONE;
}
