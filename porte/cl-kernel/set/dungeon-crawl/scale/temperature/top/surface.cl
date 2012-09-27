// @require Библиотеки из "include".


/**
* Уточняет температуру на поверхности планеты.
*
* # Предварительно надо вызвать init.cl.
*/



/**
* Инициализирует температуру на поверхности планетарной коры.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*/
inline void crustTemperature( __global temperatureCell_t t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    // # Чтобы сохранить похожесть с другими структурами, температура
    //   хранится в наборе с кол-вом ячеек = 1.

    /* - Формула упрощена. См. ниже.
    const float radius = ap->radius.core * ap->size / 2.0f;
    const float realDistance = distanceByHalfSize * ap->size / 2.0f;
    // расстояние до внешней поверхности ядра = 1.0;
    // здесь всегда <= 1.0 (см. соглашение в описании метода)
    const float distanceBySurface = realDistance / radius;
    */
    const float distanceBySurface = distanceByHalfSize / ap->radius.crust;

    // @test
    t[0].average = -11111.0f;
}




/**
* Инициализирует температуру на прилегающей к планетарной коре зоне атмосферы.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*/
inline void atmosphereTemperature( __global temperatureCell_t t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    // @test
    t[0].average = -22222.0f;
}






__kernel void surface(
    __global const aboutPlanet_t*  ap,    // 0
    __global temperatureCell_t*    tc     // 1
) {
    // всегда - координаты ячейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see Опцию -D при при построении ядра  
    const uint i = icDenorm( dnc );


    // Температуры разделены по зонам области планеты.
    // Температуру определим простой линейной функцией.
    // Работаем с реальными размерами.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    // работаем только с поверхностью планеты
    // # Планета разогревается и температура всегда распределена между
    //   двумя зонами: поверхностью и прилегающей к ней зоны.
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ) {
        crustTemperature( tc[i], ap, distanceByHalfSize );

    } else if ( interiorAtmosphereZone( ap, distanceByHalfSize ) ) {
        atmosphereTemperature( tc[i], ap, distanceByHalfSize );

    } else {
        // температура оставляем без изменений
    }


    // @test
    //tc[i].average = (distanceByHalfSize <= ap->radius.core) ? 1.0f : 2.0f;
    //tc[i].average = tc[i].dispersion = tc[i].rate = -12345.0f;
    
}
