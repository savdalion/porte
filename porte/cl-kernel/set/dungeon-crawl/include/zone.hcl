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
