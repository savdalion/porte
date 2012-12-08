#if 0
// - ������ �� ������������

/**
* ��������������� ������.
*
* ����������
*   # ������������ ������������ ����� Type4. Type3 OpenCL 1.0
*     �� ������������.
*   #! ��� ���������, ���������� ����� ���� "define" ������� ���������
*      � ��������� ���� ����. ������ ����� ��� double-��������, ������� �����
*      ��������� ����� ������ �����. � ��������� ������, ���������� � ��
*      �������� ����� ������������ ������������ ��� �������������. ������� -
*      ������.
*/


/**
* ��������� �������� � �������� ���������.
*/
/* - ��������� � �������� ���������� ��� ���������� ����.
__constant const double PRECISION = 1e-5f;
*/

inline bool zero( const double a ) {
    return  ( isless( a, PRECISION ) && isgreater( a, -PRECISION ) );
}

inline bool lZero( const double a ) {
    return isless( a, -PRECISION );
}

inline bool leZero( const double a ) {
    return lZero( a ) || zero( a );
}

inline bool gZero( const double a ) {
    return isgreater( a, PRECISION );
}

inline bool geZero( const double a ) {
    return gZero( a ) || zero( a );
}

inline bool equal( const double a, const double b ) {
    return zero( a - b );
}

inline bool positiveOne( const double a ) {
    return zero( a - 1.0f );
}

inline bool negativeOne( const double a ) {
    return zero( a + 1.0f );
}

inline bool one( const double a) {
    return positiveOne( a ) || negativeOne( a );
}



/**
* @return true, ���� �������� ����� � ��������� [a; b].
*/
inline bool between( const double v, const double a, const double b ) {
    // @todo optimize ���������� ��� ��������� ����� �������� �������.
    return (v >= a) && (v <= b);
}



/**
* @return true, ���� �������� ����� � ��������� (a; b).
*/
inline bool betweenExclude( const double v, const double a, const double b ) {
    return (v > a) && (v < b);
}

#endif
