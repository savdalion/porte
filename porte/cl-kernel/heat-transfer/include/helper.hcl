// ¬спомогательные методы


/**
* —равнение значений с заданной точностью.
*/
/* - ѕередаЄтс€ в качестве параметров при построении €дра.
__constant const float PRECISION = 1e-5f;
*/

inline bool zero( float a ) {
    return  ( isless( a, PRECISION ) && isgreater( a, -PRECISION ) );
}

inline bool lZero( float a ) {
    return isless( a, -PRECISION );
}

inline bool leZero( float a ) {
    return lZero( a ) || zero( a );
}

inline bool gZero( float a ) {
    return isgreater( a, PRECISION );
}

inline bool geZero( float a ) {
    return gZero( a ) || zero( a );
}

inline bool equal( float a, float b ) {
    return zero( a - b );
}

inline bool positiveOne( float a ) {
    return zero( a - 1 );
}

inline bool negativeOne( float a ) {
    return zero( a + 1 );
}

inline bool one( float a) {
    return positiveOne( a ) || negativeOne( a );
}





/**
* @return ѕризнак, что указанна€ точка лежит у границы (соотв. названию метода).
*         √раничные координаты передаютс€ €дру.
*/
inline bool borderRight( const int x, const int y, const int z ) {
    return (x == MAXX);
}

inline bool borderLeft( const int x, const int y, const int z ) {
    return (x == MINX);
}

inline bool borderTop( const int x, const int y, const int z ) {
    return (z == MAXZ);
}

inline bool borderBottom( const int x, const int y, const int z ) {
    return (z == MINZ);
}

inline bool borderFar( const int x, const int y, const int z ) {
    return (y == MAXY);
}

inline bool borderNear( const int x, const int y, const int z ) {
    return (y == MINY);
}

inline bool border( const int x, const int y, const int z ) {
    return borderRight(  x, y, z )
        || borderLeft(   x, y, z )
        || borderTop(    x, y, z )
        || borderBottom( x, y, z )
        || borderFar(    x, y, z )
        || borderNear(   x, y, z );
}





/**
* @return ѕризнак, что указанна€ точка лежит внутри матрицы.
* == !outsideMatrix()
*/
inline bool inside( const int x, const int y, const int z ) {
    return (
        (x >= MINX) && (x <= MAXX)
     && (y >= MINY) && (y <= MAXY)
     && (z >= MINZ) && (z <= MAXZ)
    );
}



/**
* @return ѕризнак, что указанна€ точка лежит вне матрицы N x M.
* == !insideMatrix()
*/
inline bool outside( const int x, const int y, const int z ) {
    return (
        (x < MINX) || (x > MAXX)
     || (y < MINY) || (y > MAXY)
     || (z < MINZ) || (z > MAXZ)
    );
}

