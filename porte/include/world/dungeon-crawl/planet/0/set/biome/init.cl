// @require Ѕиблиотеки из "include".


/**
* –аспознаЄт биомы в области планеты.
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

    // не можем задать фиксированное кол-во биомов, т.к. б. формируютс€ на
    // основе заданной местности
    // # “ем не менее, одна €чейка содержит не более BIOME_CELL биомов.

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
    // всегда - координаты €чейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ќпцию -D при при построении €дра  
    const uint i = icDenorm( dnc );


    // инициализируем генератор случ. чисел дл€ этой €чейки
    uint2 rstate = (uint2)( seed, i );

    // –аботаем с размерами сетки.
    const float distance = distanceNC( nc );

    // работаем только с поверхностью планеты
    // @todo ќпредел€ть биомы и в других зонах планетарной области.
    if ( exteriorCrustZone( ap, distance, nc ) ) {
        exteriorCrustBiome( bc[i], stc[i], rc[i], dc[i], lc[i], &rstate );

    } else {
        // остальные €чейки делаем нулевыми
        for (uint k = 0; k < BIOME_CELL; ++k) {
            bc[i][k].code = CB_NONE;
        }
    }

}
