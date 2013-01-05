// @require Библиотеки из "include".


/**
* Инициализирует карту температур в области планеты.
* 
* @see surface.cl
*/



/**
* @return Температура в заданной зоне области планеты.
*         Температура всегда в заданных картой "z" рамках.
*
* @param z  Карта температур в Кельвинах.
*/
inline float zoneTemperature( __global const zoneTemperature_t* z, float radius, float distanceByHalfSize ) {

    /* - Формула упрощена. См. ниже.
    const float radius = ap->radius.core * ap->size / 2.0f;
    const float realDistance = distanceByHalfSize * ap->size / 2.0f;
    // расстояние до внешней поверхности ядра = 1.0;
    // здесь всегда <= 1.0 (см. соглашение в описании метода)
    const float distanceBySurface = realDistance / radius;
    */
    const float distanceBySurface = distanceByHalfSize / radius;

    // # Из-за ошибок округления могут появляться очаги температур вне
    //   планируемого зоной диапазона. Поэтому, делаем clamp().
    
    // упорядочиваем значения
    /* - Не используется массив.
    // оптимизировано: Bool == 0 / 1
    // @see http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/isgreater.html
    const uint a = (uint)isgreater( z->center, z->surface );
    const uint b = (uint)( !a );
    */
    const bool g = isgreater( z->center, z->surface );
    const float a = g ? z->surface : z->center;
    const float b = g ? z->center  : z->surface;

    return clamp( z->center + (z->surface - z->center) * (distanceBySurface - 1.0f),  a,  b );
}




/**
* Инициализирует температуру в заданной зоне области планеты.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*/
inline void coreTemperature( __global behaviourTemperature_t* t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    // # Чтобы сохранить похожесть с другими структурами, температура
    //   хранится в наборе с кол-вом ячеек = 1.

    // @see Вычисления по карте температуры - zoneTemperature().
    t[0].average = zoneTemperature( &ap->temperature.core, ap->radius.core, distanceByHalfSize );
}



inline void mantleTemperature( __global behaviourTemperature_t* t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    // @see coreTemperature()
    t[0].average = zoneTemperature( &ap->temperature.mantle, ap->radius.core, distanceByHalfSize );
}




inline void crustTemperature( __global behaviourTemperature_t* t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    t[0].average = zoneTemperature( &ap->temperature.crust, ap->radius.core, distanceByHalfSize );
}




inline void atmosphereTemperature( __global behaviourTemperature_t* t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    t[0].average = zoneTemperature( &ap->temperature.atmosphere, ap->radius.core, distanceByHalfSize );
}




inline void spaceTemperature( __global behaviourTemperature_t* t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    t[0].average = zoneTemperature( &ap->temperature.space, ap->radius.core, distanceByHalfSize );
}






__kernel void init(
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


    // @test
    //tc[i].average = -56789.0f;
    //return;
    
    // @test
    //tc[i].average = (distanceByHalfSize <= ap->radius.core) ? 1.0f : 2.0f;

    /* @test
    uint2 rstate = (uint2)( dnx ^ dny ^ i, dny ^ dnz ^ i );
    const int tInt   = (int)uintRandom( &rstate );
    const int tIntDiapason = intDiapasonRandom( &rstate, (int2)( -100, 100 ) );
    const float tFloat = floatRandom( &rstate );
    const uint tDice = dice( &rstate, 100, 6 );
    tc[i].average    = (float)tDice;
    tc[i].dispersion = (float)tIntDiapason;
    tc[i].rate       = tFloat;
    */
    
    
    // Температуры разделены по зонам области планеты.
    // Температуру определим простой линейной функцией.
    // Работаем с реальными размерами.
    // @todo optimize? Переписать на работу по сетке? См. landscape/init.cl.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    if ( coreZone( ap, distanceByHalfSize ) ) {
        coreTemperature( tc[i], ap, distanceByHalfSize );
        
    } else if ( mantleZone( ap, distanceByHalfSize ) ) {
        mantleTemperature( tc[i], ap, distanceByHalfSize );

    } else if ( crustZone( ap, distanceByHalfSize ) ) {
        crustTemperature( tc[i], ap, distanceByHalfSize );
        
    } else if ( atmosphereZone( ap, distanceByHalfSize ) ) {
        atmosphereTemperature( tc[i], ap, distanceByHalfSize );
        
    } else {
        spaceTemperature( tc[i], ap, distanceByHalfSize );
    }
    
}
