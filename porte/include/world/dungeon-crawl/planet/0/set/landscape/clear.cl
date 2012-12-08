// @require Ѕиблиотеки из "include".


/**
* ќчищает информацию о ландшафте в области планеты.
*/
__kernel void clear(
    __global landscapeCell_t*  lc    // 0
) {
    // всегда - координаты €чейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ќпцию -D при при построении €дра  
    const uint i = icDenorm( dnc );

    
    // заполн€ем пустотой места с информацией о ландшафте
    for (uint o = 0; o < LANDSCAPE_CELL; ++o) {
        lc[i][o].code  = CEL_NONE;
        const aboutElementLandscape_t about = {};
        lc[i][o].about = about;
        lc[i][o].count = 0.0f;
        lc[i][o].scattering = 0.0f;
    }

}
