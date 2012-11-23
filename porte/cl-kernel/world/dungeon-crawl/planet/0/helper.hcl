/**
* ¬спомогательные методы.
*
* —оглашени€
*   #  оординатами обмениваемс€ через Type4. Type3 OpenCL 1.0
*     не поддерживает.
*   #! ¬се константы, получаемые извне путЄм "define" следует приводить
*      к желаемому типу €вно.  райне важно дл€ float-констант, которые вдруг
*      оказались равны целому числу. ¬ противном случае, вычислени€ с их
*      участием могут восприн€тьс€ компил€тором как целочисленные. —ловить -
*      трудно.
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
* @return true, если значение лежит в диапазоне [a; b].
*/
inline bool between( const float v, const float a, const float b ) {
    // @todo optimize ѕереписать все сравнени€ через нативные функции.
    return (v >= a) && (v <= b);
}



/**
* @return true, если значение лежит в диапазоне (a; b).
*/
inline bool betweenExclude( const float v, const float a, const float b ) {
    return (v > a) && (v < b);
}






/**
*  онстанты дл€ (де)нормализации координат.
*   # ƒолжны быть переданы €дру с параметром "-D".
*/
__constant const int4 MIN = (int4)( MIN_COORD_GRID, MIN_COORD_GRID, MIN_COORD_GRID, 0 );
__constant const int4 MAX = (int4)( MAX_COORD_GRID, MAX_COORD_GRID, MAX_COORD_GRID, 0 );



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
    return (nc.x == MAX.x);
}

inline bool borderLeft( const int4 nc ) {
    return (nc.x == MIN.x);
}

inline bool borderTop( const int4 nc ) {
    return (nc.z == MAX.z);
}

inline bool borderBottom( const int4 nc ) {
    return (nc.z == MIN.z);
}

inline bool borderFar( const int4 nc ) {
    return (nc.y == MAX.y);
}

inline bool borderNear( const int4 nc ) {
    return (nc.y == MIN.y);
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
        (nc.x >= MIN.x) && (nc.x <= MAX.x)
     && (nc.y >= MIN.y) && (nc.y <= MAX.y)
     && (nc.z >= MIN.z) && (nc.z <= MAX.z)
    );
}



/**
* @return ѕризнак, что указанна€ точка лежит вне матрицы.
* == !insideMatrix()
*/
inline bool outside( const int4 nc ) {
    return (
        (nc.x < MIN.x) || (nc.x > MAX.x)
     || (nc.y < MIN.y) || (nc.y > MAX.y)
     || (nc.z < MIN.z) || (nc.z > MAX.z)
    );
}





/**
* @return 1D-индекс по не нормализованной (+; +) 3D-координате.
*/
inline uint icDenorm( const uint4 dnc ) {
    return dnc.x + dnc.y * GRID + dnc.z * GRID * GRID;
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
* @see Ќумерацию в прим. к классу typelib::StaticMapContent3D
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








/**
* @return –ассто€ние от центра до нормализованной (-; +) 3D-координаты.
*/
inline float distanceNC( const int4 nc ) {
    return sqrt( (float)(nc.x * nc.x + nc.y * nc.y + nc.z * nc.z) );
    // @todo optimize «аменить на length( convert_float4( nc ) ).
}
