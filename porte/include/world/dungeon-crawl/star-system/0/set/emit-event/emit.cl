// @require ���������� �� "include".


/**
* �������������� ��������� ������� �������.
* ����������� � ������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




#if 0
inline void asteroidEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as
) {
    /* @test
    int w = aa->emitterEvent.waldo;
    for (uint k = 0; k < STAR_COUNT; ++k) {
        __global const aboutStar_t* ask = sc[ k ];
        const bool absent = absentStar( ask );
        if ( absent ) {
            // # ������������� ������� - ������ ����� ������.
            return;
        }
    }
    return;
    */


    // ������������ ������� ��� ���������
    // �����������
    int w = aa->emitterEvent.waldo;


    // �������� �� ������������
    const real_t coordA[ 3 ] =
        { aa->today.coord[ 0 ],  aa->today.coord[ 1 ],  aa->today.coord[ 2 ] };
    const real_t maxSide =
        max( max( aa->today.size[ 0 ], aa->today.size[ 1 ] ), aa->today.size[ 2 ] );

    for (uint k = 0; k < STAR_COUNT; ++k) {
        __global const aboutStar_t* ask = sc[ k ];
        
        // @test
        aa->emitterEvent.waldo = 1;
        return;
        
        if ( !absentStar( ask ) ) {
            // # ������������� ������� - ������ ����� ������.
            // break;  - @todo ��������. �������?
            const real_t coordB[ 3 ] =
                { ask->today.coord[ 0 ],  ask->today.coord[ 1 ],  ask->today.coord[ 2 ] };
            const real_t collisionDistance = max( maxSide, ask->today.radius );
            const bool hasCollision =
                collision( coordA,  coordB,  collisionDistance );
            //if ( hasCollision )
            {
                /*
                eventTwo_t e = {
                    // uid �������
                    E_COLLISION,
                    // ������ �������� �������
                    { GE_STAR,  k,  ask->uid },
                    {}
                };
                */
                //aa->emitterEvent.content[ w ] = e;  ++w;
                //++w;
                //aa->emitterEvent.content[ 1 ].uid = 0;
                aa->emitterEvent.waldo = 1;
            }
        }

    } // for (size_t k = 0 ...

}




inline void starEmitEvent(
    __global aboutStar_t* as
) {
    // ������������ ������� ��� ������
    int w = as->emitterEvent.waldo;
    const aboutStar_t star = *as;
    
    // ����������
    const real_t gm = G * massStar( &star );
    const eventTwo_t e = {
        // enum EVENT
        E_GRAVITY,
        // pointerElement_t - ����� �� �����
        { GE_NONE },
        // real_t fReal[ MAX_FEATURE_EVENT ]
        { gm }
    };
    as->emitterEvent.content[ w ] = e;  ++w;
    
    // �������� �� ������������
    // # ��������� ������ �������, ������ ������ ��������� �� ��������.
    //   � ������ ������ STAR ��� ��������� ASTEROID, PLANET.

    // ��������� ������
    as->emitterEvent.waldo = w;
}
#endif



// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void emit(
    __global const aboutStarSystem_t*  ass,  // 0
    __global aboutAsteroid_t*          aa,   // 1
    __global aboutPlanet_t*            ap,   // 2
    __global aboutStar_t*              as    // 3
) {
    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );
    
    // @test
    aa[ i ].uid = sizeof( aboutAsteroid_t );
    aa[ i ].tf1 = 8.8f;
    aa[ i ].tf2 = 7.7f;
    aa[ i ].td1 = (double)6.6;
    aa[ i ].td2 = (double)5.5;
    aa[ i ].emitterEvent.waldo = 7;
    if (i < ASTEROID_COUNT) {
        aa[ i ].emitterEvent.waldo = 7;
    }
    if (i < PLANET_COUNT) {
        ap[ i ].emitterEvent.waldo = 123;
    }
    if (i < STAR_COUNT) {
        as[ i ].emitterEvent.waldo = 456;
    }
    return;


    if (i < ASTEROID_COUNT) {
        //asteroidEmitEvent( ac[ i ], ac, pc, sc, i );
    }
    
    if (i < PLANET_COUNT) {
        //planetEmitEvent( pc[ i ] );
    }
    
    if (i < STAR_COUNT) {
        //starEmitEvent( sc[ i ] );
    }

}
