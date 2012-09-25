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
    #define A 4294883355U
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
    return range.x + (v % (int)(range.y - range.x + 1));
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
    float a = 0;
    for (uint i = 0; i < n; ++i) {
        const ulong v = (ulong)uintRandom( rstate );
        const ulong vm = (v % (ulong)m) + 1UL;
        a += (float)vm;
    }
    return a;
}
