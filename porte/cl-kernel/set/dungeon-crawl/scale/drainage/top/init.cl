// @require Библиотеки из "include".


/**
* Инициализирует карту дренажа в области планеты.
* 
* @see Атмосферные осадки
*/



/**
* @return Скорость дренажа в диапазоне [min, max].
*         Одинаковое значение для одинаковых параметров.
*/
inline float drainage(
    const float min,
    const float max,
    const uint seed,
    const uint i
) {
    // # Чтобы сохранить похожесть с другими структурами, осадки
    //   хранятся в наборе с кол-вом ячеек = 1.

    // метод должен возвращать одинаковое значение для одинаковых seed
    uint2 rstate = (uint2)( seed,  i );
    
    // дренаж делаем случайным
    // #i Строго говоря, с таким seed и генератором, распределение далеко от
    //    равномерного. Но фактура - интересная. Оставляем.
    const float res = (float)intDiapasonRandom( &rstate,  (int2)( min, max ) );

    return res;
}






__kernel void init(
    __global const aboutPlanet_t*  ap,    // 0
    __global drainageCell_t*       dc,    // 1
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
    const float distanceByHalfSize = distance / halfSize;

    // работаем только с поверхностью планеты
    // # Дренаж всегда распределён по поверхности.
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ||
         interiorAtmosphereZone( ap, distanceByHalfSize )
    ) {
        // диапазон расшириряем, т.к. скорость дренажа задана средняя
        const float min = ap->drainage.crust.min * 0.5f;
        const float max = ap->drainage.crust.max * 1.5f;
        dc[i][0].average = drainage( min, max, seed, i );

    } else {
        // остальные ячейки делаем нулевыми
        dc[i][0].average = 0.0f;
    }

}
