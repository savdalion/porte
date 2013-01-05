// @require helper.hcl


/**
* ћетоды помогают определить зону области планеты по указанным
* координатам.
*
* @see —оглашени€ в helper.hcl.
*/



/**
* @return «аданна€ точка сетки находитс€ в зоне €дра области планеты.
*
* ѕример вызова метода
    const float halfSize = ap->size / 2.0f;
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float distanceByHalfSize = distance / halfSize;
    if ( coreZone( ap, distanceByHalfSize ) ) {
        // ...
    }
* где ap - информаци€ из структуры aboutPlanet_t,
*     nc - нормализованные координаты (см. normalizeCoord()),
*     SCALE - сколько метров занимает €чейка рабочей сетки (передаЄтс€
*             в качестве параметра при вызове €дра).
*/
inline bool coreZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.core);
}


/**
* @return «аданна€ точка сетки находитс€ в зоне мантии области планеты.
*/
inline bool mantleZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.mantle);
}


/**
* @return «аданна€ точка сетки находитс€ в зоне коры области планеты.
*/
inline bool crustZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.crust);
}


/**
* @return «аданна€ точка сетки находитс€ в зоне атмосферы области планеты.
*/
inline bool atmosphereZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.atmosphere);
}


/**
* @return «аданна€ точка сетки находитс€ выше атмосферы области планеты.
*/
inline bool spaceZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize > ap->radius.atmosphere);
}




/**
* @return «аданна€ точка сетки находитс€ на внешней поверхности планетарной коры.
*/
inline bool exteriorCrustZone( __global const aboutPlanet_t* ap, float distance, int4 nc ) {
    // обеспечиваем непрерывность сферы
    // # «десь не помешает хранение 'static', но CPU OpenCL не воспринимает его.
    const float K = 2.5f;
    const float outerSphere = (float)GRID / 2.0f * ap->radius.crust;
    const float innerSphere = ((float)GRID / 2.0f - K) * ap->radius.crust;

    return (distance <= outerSphere) && (distance > innerSphere);
}



/**
* @return «аданна€ точка сетки находитс€ на внутренней поверхности атмосферы планеты.
*/
inline bool interiorAtmosphereZone( __global const aboutPlanet_t* ap, float distance, int4 nc ) {
    // обеспечиваем непрерывность сферы
    // # «десь не помешает хранение 'static', но CPU OpenCL не воспринимает его.
    const float K = 2.5f;
    const float outerSphere = ((float)GRID / 2.0f + K) * ap->radius.crust;
    const float innerSphere = (float)GRID / 2.0f * ap->radius.crust;

    return (distance <= outerSphere) && (distance > innerSphere);
}
