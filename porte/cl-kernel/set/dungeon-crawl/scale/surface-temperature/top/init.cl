// @require Библиотеки из "include".


/**
* Задаёт температуру на поверхности планеты.
*/



/**
* Инициализирует температуру на поверхности планетарной коры.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*/
inline void exteriorCrustTemperature( __global surfaceTemperatureCell_t t, __global const aboutPlanet_t* ap, const int4 nc ) {
    // # Чтобы сохранить похожесть с другими структурами, температура
    //   хранится в наборе с кол-вом ячеек = 1.
    
    // примерное расстояние наблюдаемой ячейки до экватора
    //  1.0  северный полюс
    //  0.0  экватор
    // -1.0  южный полюс
    const float ed = (float)nc.z / (float)MAX_COORD_GRID;

    // температура на поверхности
    const float pole = ap->surfaceTemperature.crust.pole;
    const float equator = ap->surfaceTemperature.crust.equator;
    t[0].average = equator + (pole - equator) * fabs( ed );
}




/**
* Инициализирует температуру на прилегающей к планетарной коре зоне атмосферы.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*/
inline void interiorAtmosphereTemperature( __global surfaceTemperatureCell_t t, __global const aboutPlanet_t* ap, const int4 nc ) {
    // @see exteriorCrustTemperature()
    const float ed = (float)nc.z / (float)MAX_COORD_GRID;
    const float pole = ap->surfaceTemperature.crust.pole;
    const float equator = ap->surfaceTemperature.crust.equator;
    t[0].average = equator + (pole - equator) * fabs( ed );
}






__kernel void init(
    __global const aboutPlanet_t*       ap,    // 0
    __global surfaceTemperatureCell_t*  stc    // 1
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
    // # Планета разогревается и температура всегда распределена между
    //   двумя зонами: поверхностью и прилегающей к ней зоны.
    //   Причина: т.о. будет проще и быстрее определить реальную температуру в
    //   конкретной ячейке портулана при сравнении с данными в "temperature".
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ) {
        exteriorCrustTemperature( stc[i], ap, nc );

    } else if ( interiorAtmosphereZone( ap, distanceByHalfSize ) ) {
        interiorAtmosphereTemperature( stc[i], ap, nc );

    } else {
        // остальную температуру делаем абсолютным нулём
        stc[i][0].average = 0.0f;
    }

}
