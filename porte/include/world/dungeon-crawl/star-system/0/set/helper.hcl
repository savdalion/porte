/**
* ��������������� ������.
*
* @typedef real_t  ����� ��� ������������ ��� ������������� ������������ �����.
*
* @define PRECISION   �������� ��������� ������������ �����.
* @define GPU_OPENCL  ���� ���� ����������� �� GPU, ������ ����� ���������
*         �������� �������. �������: � CPU �� ����������� ��������� �������.
*
* ����������
*   # ������������ ������������ ����� Type4. Type3 OpenCL 1.0
*     �� ������������.
*   #! ��� ���������, ���������� ����� ���� "define" ������� ���������
*      � ��������� ���� ����. ������ ����� ��� real_t-��������, ������� �����
*      ��������� ����� ������ �����. � ��������� ������, ���������� � ��
*      �������� ����� ������������ ������������ ��� �������������. ������� -
*      ������.
*/


/**
* ��������� �������� � �������� ���������.
*/
inline bool zero( const real_t a ) {
#ifdef GPU_OPENCL
    return ( isless( a, PRECISION ) && isgreater( a, -PRECISION ) );
#else
    return ( (a < PRECISION) && (a > -PRECISION) );
#endif
}


inline bool lZero( const real_t a ) {
#ifdef GPU_OPENCL
    return isless( a, 0.0f );
#else
    return (a < 0.0f);
#endif
}


inline bool leZero( const real_t a ) {
#ifdef GPU_OPENCL
    return isless( a, PRECISION );
#else
    return (a < PRECISION);
#endif
}


inline bool gZero( const real_t a ) {
#ifdef GPU_OPENCL
    return isgreater( a, 0.0f );
#else
    return (a > 0.0f);
#endif
}


inline bool geZero( const real_t a ) {
#ifdef GPU_OPENCL
    return isgreater( a, -PRECISION );
#else
    return (a > PRECISION);
#endif
}


inline bool equal( const real_t a, const real_t b ) {
    return zero( a - b );
}


inline bool positiveOne( const real_t a ) {
    return zero( a - 1.0f );
}


inline bool negativeOne( const real_t a ) {
    return zero( a + 1.0f );
}


inline bool one( const real_t a) {
    return positiveOne( a ) || negativeOne( a );
}




/**
* @return true, ���� �������� ����� � ��������� [a; b].
*/
inline bool betweenReal( const real_t v, const real_t a, const real_t b ) {
    // @todo optimize ���������� ��� ��������� ����� �������� �������.
    return (v >= a) && (v <= b);
}


inline bool betweenInteger( const int v, const int a, const int b ) {
    return (v >= a) && (v <= b);
}




/**
* @return true, ���� �������� ����� � ��������� (a; b).
*/
inline bool betweenRealExclude( const real_t v, const real_t a, const real_t b ) {
    return (v > a) && (v < b);
}


inline bool betweenIntegerExclude( const int v, const int a, const int b ) {
    return (v > a) && (v < b);
}




/**
* assert() ��� OpenCL.
* ���������� ��������� � ��� ���������� ����������.
*/
