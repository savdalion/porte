// @require Библиотеки из "include".


/**
* Инициализирует карту атмосферных осадков в области планеты.
* 
* @see Дренаж
*/



/**
* @return Кол-во атм. осадков в диапазоне [min, max].
*         Одинаковое значение для одинаковых параметров.
*/
inline float rainfall(
    const float min,
    const float max,
    const uint seed,
    const int4 nc,
    const uint i
) {
    // # Чтобы сохранить похожесть с другими структурами, осадки
    //   хранятся в наборе с кол-вом ячеек = 1.

    // # Ближе к экватору выпадает больше осадков.
    // примерное расстояние наблюдаемой ячейки до экватора
    //  1.0  северный полюс
    //  0.0  экватор
    // -1.0  южный полюс
    const float ed = (float)nc.z / (float)MAX_COORD_GRID;

    // метод должен возвращать одинаковое значение для одинаковых seed
    uint2 rstate = (uint2)( seed,  i );
    
    // вариант 1
    //static const float P = 100.0f;
    //const float a = (float)intDiapasonRandom( &rstate, (int2)( min * P, max * P ) ) / P;
    //const float a = floatDiapasonRandom( &rstate, (float2)( min, max ) );
    
    // вариант 2
    // больше осадков выпадает в южном полушарии
    // @see Дренаж.
    const float k = floatRandom( &rstate ) - 0.3f;
    const float res = max + (min - max) * fabs( ed + k );

    return clamp( res, min, max );
}







__kernel void init(
    __global const aboutPlanet_t*  ap,    // 0
    __global rainfallCell_t*       rc,    // 1
    const uint                   seed     // 2
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
    // @todo optimize Считать 1 / halfSize перед вызовом ядра. См. и др. ядра.
    const float distanceByHalfSize = distance / halfSize;

    // работаем только с поверхностью планеты
    // # Атмосферные осадки всегда распределены между
    //   двумя зонами: поверхностью и прилегающей к ней зоне.
    //   Причина: т.о. будет проще и быстрее определить осадки в
    //   конкретной ячейке портулана.
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ||
         interiorAtmosphereZone( ap, distanceByHalfSize )
    ) {
        // диапазон расшириряем, т.к. кол-во осадков задано среднее
        const float min = ap->rainfall.crust.min * 0.5f;
        const float max = ap->rainfall.crust.max * 1.5f;
        rc[i][0].average = rainfall( min, max, seed, nc, i );

    } else {
        // остальные ячейки делаем нулевыми
        rc[i][0].average = 0.0f;
    }

}
