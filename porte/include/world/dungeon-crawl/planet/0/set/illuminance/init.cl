// @require Библиотеки из "include".


/**
* Задаёт освещённость в области планеты.
*/




// @define illuminance-utils.h
// @todo fine Разобраться, почему нельзя объявить метод только в illuminance-utils.h.
void starIlluminance(
    __global illuminanceCell_t  u,
    // планета
    const cl_float  radius,
    const cl_float  coord[ 3 ],
    const cl_float  omega[ 3 ]
    // звезда
    const cl_float  radiusStar,
    const cl_float  coordStar[ 3 ],
    const cl_float  luminousIntensityStar
);




/**
* @param coord  Координаты планеты в звёздной системе.
* @param omega  Угол наклона планеты по осям XYZ.
* @param ai     Информация об источниках освещения планеты.
*/
__kernel void init(
    __global const aboutPlanet_t*       ap,       // 0
    __global const cl_float*            coord     // 1
    __global const cl_float*            omega     // 2
    __global const aboutIlluminance_t*  ai        // 3
    __global illuminanceCell_t*         uc        // 4
) {
    // всегда - координаты ячейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see Опцию -D при при построении ядра  
    const uint i = icDenorm( dnc );


    // работаем со всей областью планеты
    // обнуляем освещённость - посчитаем с нуля
    uc[ i ][ 0 ].star = 0.0f;

    // пройдём по всем источникам освещения, наполним карту освещённости
    // свет от звёзд
    for (uint k = 0; k < ABOUT_ILLUMINANCE_STAR_COUNT; ++k) {
        starIlluminance(
            uc[ i ],
            // планета
            ap->radius, coord, omega,
            // звезда
            ai.star[ k ].radius,
            ai.star[ k ].coord,
            ai.star[ k ].luminousIntensity
        );
    }

    // @todo См. aboutIlluminance_t, illuminance

}
