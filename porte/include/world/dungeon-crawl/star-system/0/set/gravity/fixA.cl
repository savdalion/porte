// @require Библиотеки из "include".


/**
* Переносит данные, накопленные во временной структуре при
* выполнении 'pulseA', в постоянную структуру.
*/
__kernel void fixA(
    __global       aboutBody_t*  b,  // 0
    __global const aboutBody_t*  wb  // 1
) {
    const uint i = get_global_id( 0 );

    // # Все данные из 'wb' актуальны для 'b'. См. соглашение в pulseA.cl.
    b[ i ] = wb[ i ];

}
