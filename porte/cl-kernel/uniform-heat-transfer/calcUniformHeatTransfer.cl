#include "include/pragma.hcl"
#include "include/struct.hcl"
#include "include/helper.hcl"


/**
* Считает распространение тепла (температуры) в заданном объёме
* для однородной среды.
*/
__kernel void calcUniformHeatTransfer(
    __global booster_t* b,    // 0
    __global booster_t* wb    // 1
) {
    // всегда - координаты ячейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see Опцию -D при при построении ядра  
    const uint i = icDenorm( dnc );

    
    /* @test
    wb->temperature[i] = -b->temperature[i];
    b->temperature[i] = wb->temperature[i];
    */
    
    
    // Работаем с 1-й ячейкой.
    // Найдём среднее арифметическое температуры вокруг ячейки, включая
    // саму ячейку. Просмотрим первые Q сторон (см. прим. к isc()).
    const uint Q = 6;
    const uint l[ 6 ] = { 1, 2, 3, 4, 9, 18 };

    float at = b->temperature[ i ];
    for (uint j = 0; j < Q; ++j) {
        const int4 u = nc + isc( l[ j ] );        

#if 0
        // (2) если сосед за границей, считаем его температуру равной
        // температуре самой ячейки
        // Температура выравнивается, нет необходимости в дополнительном тюнинге.
        // @todo optimize? Задействовать select().
        if ( outside( u ) ) {
            at += b->temperature[ i ];
        } else {
            const uint4 dnu = denormalizeCoord( u );
            const uint iu = icDenorm( dnu );
            at += b->temperature[ iu ];
        } 

#else
        // (2) быстрое(?) грязное усреднение
        // (i) Ощущения "быстрее варианта (1)" не возникает, отнюдь.
        const uint4 dnu = denormalizeCoord( u );
        const uint iu = icDenorm( dnu );
        at += b->temperature[ (iu < NML) ? iu : i ];
#endif
        
    } // for (uint k

    const float averageT = at / (float)( Q + 1 );

    


#if 0
// @test
    const uint Q = 6 + 1;

    // #0
    float at = b->temperature[ i ];
    
    // #1
    int4 u = (int4)( nc.x + 0,  nc.y + 0,  nc.z + 1,  0 );
    if ( outside( u ) ) {
        at += b->temperature[ i ];
    } else {
        const uint4 dnu = denormalizeCoord( u );
        const uint ki = icDenorm( dnu );
        at += b->temperature[ ki ];
    } 

    
    // #2
    u = (int4)( nc.x + 1,  nc.y + 0,  nc.z + 0,  0 );
    if ( outside( u ) ) {
        at += b->temperature[ i ];
    } else {
        const uint4 dnu = denormalizeCoord( u );
        const uint ki = icDenorm( dnu );
        at += b->temperature[ ki ];
    } 
    
    // #3
    u = (int4)( nc.x + 0,  nc.y + 0,  nc.z - 1,  0 );
    if ( outside( u ) ) {
        at += b->temperature[ i ];
    } else {
        const uint4 dnu = denormalizeCoord( u );
        const uint ki = icDenorm( dnu );
        at += b->temperature[ ki ];
    } 
    
    // #4
    u = (int4)( nc.x - 1,  nc.y + 0,  nc.z + 0,  0 );
    if ( outside( u ) ) {
        at += b->temperature[ i ];
    } else {
        const uint4 dnu = denormalizeCoord( u );
        const uint ki = icDenorm( dnu );
        at += b->temperature[ ki ];
    }
    
    // #9
    u = (int4)( nc.x + 0,  nc.y - 1,  nc.z + 0,  0 );
    if ( outside( u ) ) {
        at += b->temperature[ i ];
    } else {
        const uint4 dnu = denormalizeCoord( u );
        const uint ki = icDenorm( dnu );
        at += b->temperature[ ki ];
    }
    
    // #18
    u = (int4)( nc.x + 0,  nc.y + 1,  nc.z + 0,  0 );
    if ( outside( u ) ) {
        at += b->temperature[ i ];
    } else {
        const uint4 dnu = denormalizeCoord( u );
        const uint ki = icDenorm( dnu );
        at += b->temperature[ ki ];
    }
    
    const float averageT = at / (float)( Q );
#endif
    
    
    // Запишем результат во временную таблицу, чтобы обновить
    // после полной синхронизации
    wb->temperature[i] = averageT;

}
