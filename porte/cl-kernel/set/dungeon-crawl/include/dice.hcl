/**
* Генераторы случайных чисел.
*/


/**
* Период конкретно этого генератора = 2^63.
*
* @source http://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
*/
inline uint uintRandom( uint2* rstate ) {
    //enum { A = 4294883355U };
    #define A (4294883355U)
    uint x = (*rstate).x,  c = (*rstate).y;
    const uint res = x ^ c;
    const uint hi = mul_hi(x, A);
    x = x * A + c;
    c = hi + (x < c);
    *rstate = (uint2)(x, c);
    #undef A
    return res;
}




inline float floatRandom( uint2* rstate ) {
    return (float)( uintRandom( rstate ) ) / (float)( 0xFFFFFFFF );
}




inline int intDiapasonRandom( uint2* rstate, int2 range ) {
    // @source http://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
    const uint v = uintRandom( rstate );
    return range.x + (v % (range.y - range.x + 1));
}




inline float floatDiapasonRandom( uint2* rstate, float2 range ) {
    const float v = floatRandom( rstate );
    return range.x + (v * (range.y - range.x + 1.0f));
}



/**
* @return Значение в диапазоне [ center * (1.0 - deviation);  center * (1.0 + deviation) ].
*/
inline float floatDeviationRandom( uint2* rstate, float center, float deviation ) {
    return floatDiapasonRandom( rstate,  (float2)( center * (1.0 - deviation),  center * (1.0 + deviation) ) );
}




/**
* Бросок кости "nDm".
*/
inline uint dice( uint2* rstate, uint n, uint m ) {
    uint a = 0;
    for (uint i = 0; i < n; ++i) {
        const uint v = uintRandom( rstate );
        const uint vm = (v % m) + 1;
        a += vm;
    }
    return a;
}



inline float floatDice( uint2* rstate, uint n, float m ) {
    /* - Переписано. См. ниже.
    float a = 0;
    // кол-во нулей определяет кол-во десятичных знаков
    #define P (100000UL)
    // минимальное приращение при каждом броске = 1 / P
    const ulong M = (ulong)( m * P );
    for (uint i = 0; i < n; ++i) {
        const ulong v = (ulong)uintRandom( rstate );
        const ulong vm = (v % M) + 1UL;
        a += (float)vm / (float)P;
    }
    #undef P
    return a;
    */

    float a = 0;
    for (uint i = 0; i < n; ++i) {
        const float vm = floatRandom( rstate ) * m + PRECISION;
        a += vm;
    }
    return a;
}
