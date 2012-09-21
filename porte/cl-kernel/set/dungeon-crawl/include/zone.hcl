/**
* Методы помогают определить зону области планеты по указанным
* координатам.
*
* @see Соглашения в helper.hcl.
*/



/**
* @return Заданная точка сетки находится в зоне ядра области планеты.
*
* Пример вызова метода
    const float halfSize = ap->size / 2.0f;
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float distanceByHalfSize = distance / halfSize;
    if ( coreZone( ap, distanceByHalfSize ) ) {
        // ...
    }
* где ap - информация из структуры aboutPlanet_t,
*     nc - нормализованные координаты (см. normalizeCoord()),
*     SCALE - сколько метров занимает ячейка рабочей сетки (передаётся
*             в качестве параметра при вызове ядра).
*/
inline bool coreZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.core);
}


/**
* @return Заданная точка сетки находится в зоне мантии области планеты.
*/
inline bool mantleZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.mantle);
}


/**
* @return Заданная точка сетки находится в зоне коры области планеты.
*/
inline bool crustZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.crust);
}


/**
* @return Заданная точка сетки находится в зоне атмосферы области планеты.
*/
inline bool atmosphereZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.atmosphere);
}


/**
* @return Заданная точка сетки находится выше атмосферы области планеты.
*/
inline bool spaceZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize > ap->radius.atmosphere);
}





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
    // оптимизировано: Bool == 0 / 1
    // @see http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/isgreater.html
    const uint a = (uint)isgreater( z->map[0], z->map[1] );
    const uint b = (uint)( !a );

    return clamp( z->map[0] + (z->map[1] - z->map[0]) * (distanceBySurface - 1.0f),  z->map[a],  z->map[b] );
}
