// @require Библиотеки из "include".


/**
* Распознаёт биомы в области планеты.
*/



__kernel void init(
    __global const aboutPlanet_t*             ap,    // 0
    __global biomeCell_t*                     bc,    // 1
    __global const temperatureCell_t*         tc,    // 2
    __global const surfaceTemperatureCell_t*  stc,   // 3
    __global const rainfallCell_t*            rc,    // 4
    __global const drainageCell_t*            dc,    // 5
    const uint                                seed   // 6
) {
    // всегда - координаты ячейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see Опцию -D при при построении ядра  
    const uint i = icDenorm( dnc );


    // Работаем с реальными размерами.

    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    // работаем только с поверхностью планеты
    // @todo Определять биомы и в других зонах планетарной области.
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ) {
        biome( bc[i], tc[i], stc[i], rc[i], dc[i], seed, i );

    } else {
        // остальные ячейки делаем нулевыми
        for (uint k = 0; k < BIOME_CELL; ++k) {
            bc[i][k].code = CB_NONE;
        }
    }

}
