// @require ���������� �� "include".


/**
* �������������� �������� ������� ������� � ����� ���������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� direct(), relative(), fix().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




inline void asteroidBeginEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutAsteroid_t* aai = &aa[ i ];
    __global emitterEvent_t* ee = &aai->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n", i, ee->waldo );
    }
#endif


    // �������� �������
    /* - ���. ���������� �������� ������.
    for (int w = ee->waldo; w > 0; --w) {
        ee->content[ w ].uid = E_NONE;
    }
    */

    // �������� ������
    ee->waldo = 0;
}








inline void starBeginEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutStar_t* asi = &as[ i ];
    __global emitterEvent_t* ee = &asi->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "Star %d is not initialized or it memory is overfilled. Waldo = %i.\n", i, ee->waldo );
    }
#endif


    // �������� �������
    /* - ���. ���������� �������� ������.
    for (int w = ee->waldo; w > 0; --w) {
        ee->content[ w ].uid = E_NONE;
    }
    */

    // �������� ������
    ee->waldo = 0;
}




// @see world/dungeon-crawl/star-system/0/set/structure.h
// @todo fine optimize ���������� ������� ������ ����������� �� ���������.
__kernel void begin(
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    const real_t                       timestep  // 4
) {
    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );


    if ( (i < ASTEROID_COUNT) && presentAsteroid( &aa[ i ] ) ) {
        asteroidBeginEmitEvent( i, aa, ap, as, timestep );
    }

    if ( (i < PLANET_COUNT) && presentPlanet( &ap[ i ] ) ) {
        //planetBeginEmitEvent( pc[ i ] );
    }

    if ( (i < STAR_COUNT) && presentStar( &as[ i ] ) ) {
        starBeginEmitEvent( i, aa, ap, as, timestep );
    }

}
