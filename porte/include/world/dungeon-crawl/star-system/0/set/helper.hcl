#if 0
// - сейчас не используются

/**
* Вспомогательные методы.
*
* Соглашения
*   # Координатами обмениваемся через Type4. Type3 OpenCL 1.0
*     не поддерживает.
*   #! Все константы, получаемые извне путём "define" следует приводить
*      к желаемому типу явно. Крайне важно для double-констант, которые вдруг
*      оказались равны целому числу. В противном случае, вычисления с их
*      участием могут восприняться компилятором как целочисленные. Словить -
*      трудно.
*/


/**
* Сравнение значений с заданной точностью.
*/
/* - Передаётся в качестве параметров при построении ядра.
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
* @return true, если значение лежит в диапазоне [a; b].
*/
inline bool between( const double v, const double a, const double b ) {
    // @todo optimize Переписать все сравнения через нативные функции.
    return (v >= a) && (v <= b);
}



/**
* @return true, если значение лежит в диапазоне (a; b).
*/
inline bool betweenExclude( const double v, const double a, const double b ) {
    return (v > a) && (v < b);
}

#endif
