// @require Библиотеки из "include".


/**
* Переносит данные из 'future' в 'today'.
*
* @see emit.cl
*/
__kernel void fix(
    __global aboutAsteroid_t*  aa,   // 0
    __global aboutPlanet_t*    ap,   // 1
    __global aboutStar_t*      as    // 2
) {
    const uint i = get_global_id( 0 );

    // @todo ...

}
