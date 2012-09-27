/**
* ¬спомогательные методы.
*
* —оглашени€
*   #  оординатами обмениваемс€ через Type4. Ќе Type3, т.к. OpenCL 1.0
*     не поддерживает.
*/



/**
* —равнение значений с заданной точностью.
*/
/* - ѕередаЄтс€ в качестве параметров при построении €дра.
__constant const float PRECISION = 1e-5f;
*/

inline bool zero( const float a ) {
    return  ( isless( a, PRECISION ) && isgreater( a, -PRECISION ) );
}

inline bool lZero( const float a ) {
    return isless( a, -PRECISION );
}

inline bool leZero( const float a ) {
    return lZero( a ) || zero( a );
}

inline bool gZero( const float a ) {
    return isgreater( a, PRECISION );
}

inline bool geZero( const float a ) {
    return gZero( a ) || zero( a );
}

inline bool equal( const float a, const float b ) {
    return zero( a - b );
}

inline bool positiveOne( const float a ) {
    return zero( a - 1.0f );
}

inline bool negativeOne( const float a ) {
    return zero( a + 1.0f );
}

inline bool one( const float a) {
    return positiveOne( a ) || negativeOne( a );
}





/**
*  онстанты дл€ (де)нормализации координат.
*/
__constant const int4 MIN = (int4)( MINX, MINY, MINZ, 0 );
__constant const int4 MAX = (int4)( MAXX, MAXY, MAXZ, 0 );



/**
* @return Ќормализованна€ 3D-координата (значени€ в диапазоне [-MAXx/y/z; +MAXx/y/z]).
*/
inline int4 normalizeCoord( const uint4 dnc ) {
    /*
    const int4 nc = (int4)( dnc.x, dnc.y, dnc.z, 0 ) - MAX;
    return nc;
    */
    return (int4)( (int)dnc.x + MIN.x,  (int)dnc.y + MIN.y,  (int)dnc.z + MIN.z,  0 );
}



/**
* @return ƒенормализованна€ 3D-координата (значени€ в диапазоне [0; n/m/l]).
*/
inline uint4 denormalizeCoord( const int4 nc ) {
    /*
    const int4 t = nc + MAX;
    return (uint4)( t.x, t.y, t.z, 0 );
    */
    return (uint4)( nc.x + MAX.x,  nc.y + MAX.y,  nc.z + MAX.z,  0 );
}






/**
* @return ѕризнак, что указанна€ точка лежит у границы (соотв. названию метода).
*         √раничные координаты передаютс€ €дру.
*/
inline bool borderRight( const int4 nc ) {
    return (nc.x == MAXX);
}

inline bool borderLeft( const int4 nc ) {
    return (nc.x == MINX);
}

inline bool borderTop( const int4 nc ) {
    return (nc.z == MAXZ);
}

inline bool borderBottom( const int4 nc ) {
    return (nc.z == MINZ);
}

inline bool borderFar( const int4 nc ) {
    return (nc.y == MAXY);
}

inline bool borderNear( const int4 nc ) {
    return (nc.y == MINY);
}

inline bool border( const int4 nc ) {
    return borderRight(  nc )
        || borderLeft(   nc )
        || borderTop(    nc )
        || borderBottom( nc )
        || borderFar(    nc )
        || borderNear(   nc );
}





/**
* @return ѕризнак, что указанна€ точка лежит внутри матрицы.
* == !outsideMatrix()
*/
inline bool inside( const int4 nc ) {
    return (
        (nc.x >= MINX) && (nc.x <= MAXX)
     && (nc.y >= MINY) && (nc.y <= MAXY)
     && (nc.z >= MINZ) && (nc.z <= MAXZ)
    );
}



/**
* @return ѕризнак, что указанна€ точка лежит вне матрицы N x M.
* == !insideMatrix()
*/
inline bool outside( const int4 nc ) {
    return (
        (nc.x < MINX) || (nc.x > MAXX)
     || (nc.y < MINY) || (nc.y > MAXY)
     || (nc.z < MINZ) || (nc.z > MAXZ)
    );
}





/**
* @return 1D-индекс по не нормализованной (+; +) 3D-координате.
*/
inline uint icDenorm( const uint4 dnc ) {
    return dnc.x + dnc.y * N + dnc.z * NM;
}



/**
* @return 1D-индекс по нормализованной (-; +) 3D-координате.
*/
inline uint ic( const int4 nc ) {
    const uint4 dnc = denormalizeCoord( nc );
    return icDenorm( dnc );
}




/**
* @return ¬ектор смещени€ в сторону указанной €чейки.
*
* @see Ќумерацию в прим. к классу typelib::MapContent3D
*      https://github.com/savdalion/typelib
*/
inline int4 isc( const uint cell ) {
    static const int4 a[27] = {
        // центральный 2D слой: 0-8
        (int4)(  0,  0,  0,  0 ),
        (int4)(  0,  0, +1,  0 ),
        (int4)( +1,  0,  0,  0 ),
        (int4)(  0,  0, -1,  0 ),
        (int4)( -1,  0,  0,  0 ),
        (int4)( +1,  0, +1,  0 ),
        (int4)( +1,  0, -1,  0 ),
        (int4)( -1,  0, -1,  0 ),
        (int4)( -1,  0, +1,  0 ),
        // ближний 2D слой: 9-17
        (int4)(  0, -1,  0,  0 ),
        (int4)(  0, -1, +1,  0 ),
        (int4)( +1, -1,  0,  0 ),
        (int4)(  0, -1, -1,  0 ),
        (int4)( -1, -1,  0,  0 ),
        (int4)( +1, -1, +1,  0 ),
        (int4)( +1, -1, -1,  0 ),
        (int4)( -1, -1, -1,  0 ),
        (int4)( -1, -1, +1,  0 ),
        // дальний 2D слой: 18-26
        (int4)(  0, +1,  0,  0 ),
        (int4)(  0, +1, +1,  0 ),
        (int4)( +1, +1,  0,  0 ),
        (int4)(  0, +1, -1,  0 ),
        (int4)( -1, +1,  0,  0 ),
        (int4)( +1, +1, +1,  0 ),
        (int4)( +1, +1, -1,  0 ),
        (int4)( -1, +1, -1,  0 ),
        (int4)( -1, +1, +1,  0 )
    };

    return a[ cell ];
}
