// @require ���������� �� "include".


/**
* ��������� �������������� �������� ������� ������� �������� ��������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� direct(), relative().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




inline void asteroidFixEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutAsteroid_t* aai = &aa[ i ];
    __global const emitterEvent_t* ee = &aai->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n", i, ee->waldo );
    }
#endif


    // ������ �������������� �. � ����������� �� �������
    // # �� ���������� �� ������� �������: ���� ��� ������-��
    //   ��������� � ����� ��������.
    for (int w = ee->waldo - 1; w >= 0; --w) {
        __global const eventTwo_t* e = &ee->content[ w ];
        const enum EVENT uid = e->uid;
        // # ����������� �������� enum Event.
        switch ( uid ) {
            case E_NONE :
                // ������ ������, ������ �������
                break;

            case E_COLLISION :
                // # ��������� �������.
                // ������� ������������� ��� ����������
                break;

            case E_CHANGE_COORD :
                addCoord1( &aai->today.coord[ 0 ],  e->fReal[ 0 ] );
                addCoord1( &aai->today.coord[ 1 ],  e->fReal[ 1 ] );
                addCoord1( &aai->today.coord[ 2 ],  e->fReal[ 2 ] );
                break;

            case E_CHANGE_MASS :
                addMass( &aai->today.mass,  e->fReal[ 1 ] );
                break;

            case E_CHANGE_TEMPERATURE :
                aai->today.temperature += e->fReal[ 1 ];
                break;

            case E_CHANGE_VELOCITY :
                aai->today.velocity[ 0 ] += e->fReal[ 0 ];
                aai->today.velocity[ 1 ] += e->fReal[ 1 ];
                aai->today.velocity[ 2 ] += e->fReal[ 2 ];
                break;

            case E_CRUSH_N :
                // @todo ...
                break;

            case E_DESTROY :
#ifdef __DEBUG
                printf( "fix() Destroy asteroid %d.\n", aai->uid );
#endif
                aai->today.live = false;
                break;

            case E_GRAVITY :
                // # �������� ������� ���, ����� ������������� ��������������
                //   �� ������ ��������.
                // ������� ������������� ��� ����������
                break;

            case E_IMPACT_ACCELERATION :
                // ������� ������������� ��� ����������
                break;

            case E_IMPACT_FORCE :
                // ������� ������������� ��� ����������
                break;

            case E_RADIATION :
                // ������� ������������� ��� ����������
                break;

            case E_last :
                // ������ ������� �� ���������� �����������
                break;
        }

    } // for (int w = ...

}








inline void starFixEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutStar_t* asi = &as[ i ];
    __global const emitterEvent_t* ee = &asi->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "Star %d is not initialized or it memory is overfilled. Waldo = %i.\n", i, ee->waldo );
    }
#endif


    // ������ �������������� �. � ����������� �� �������
    // # �� ���������� �� ������� �������: ���� ��� ������-��
    //   ��������� � ����� ��������.
    for (int w = ee->waldo - 1; w >= 0; --w) {
        __global const eventTwo_t* e = &ee->content[ w ];
        const enum EVENT uid = e->uid;
        // # ����������� �������� enum Event.
        switch ( uid ) {
            case E_NONE :
                // ������ ������, ������ �������
                break;

            case E_COLLISION :
                // # ��������� �������.
                // ������� ������������� ��� ����������
                break;

            case E_CHANGE_COORD :
                /* - @todo
                addCoord1( &asi->today.coord[ 0 ],  e->fReal[ 0 ] );
                addCoord1( &asi->today.coord[ 1 ],  e->fReal[ 1 ] );
                addCoord1( &asi->today.coord[ 2 ],  e->fReal[ 2 ] );
                */
                asi->today.coord[ 0 ] += e->fReal[ 0 ];
                asi->today.coord[ 1 ] += e->fReal[ 1 ];
                asi->today.coord[ 2 ] += e->fReal[ 2 ];
                break;

            case E_CHANGE_MASS :
#ifdef __DEBUG
                if (e->fReal[ 1 ] > 0) {
                    printf( "fix() Change mass star %d on %e kg.", asi->uid, e->fReal[ 1 ] );
                }
#endif
                addMass( &asi->today.mass,  e->fReal[ 1 ] );
#ifdef __DEBUG
                if (e->fReal[ 1 ] > 0) {
                    printf( " Mass star is %e kg.\n", massStar( asi ) );
                }
#endif
                break;

            case E_CHANGE_TEMPERATURE :
                asi->today.surfaceTemperature += e->fReal[ 1 ];
                break;

            case E_CHANGE_VELOCITY :
                asi->today.velocity[ 0 ] += e->fReal[ 0 ];
                asi->today.velocity[ 1 ] += e->fReal[ 1 ];
                asi->today.velocity[ 2 ] += e->fReal[ 2 ];
                break;

            case E_CRUSH_N :
                // @todo ...
                break;

            case E_DESTROY :
#ifdef __DEBUG
                printf( "fix() Destroy star %d.\n", asi->uid );
#endif
                asi->today.live = false;
                break;

            case E_GRAVITY :
                // # ���������� ������ ������ ������ �� �������� ������� �������.
                break;

            case E_IMPACT_ACCELERATION :
                // ������� ������������� ��� ����������
                break;

            case E_IMPACT_FORCE :
                // ������� ������������� ��� ����������
                break;

            case E_RADIATION :
                // # ������ ����� ��� ��������� ��� ���� ������������ ���
                //   ��������� ������� � ���� direct() � ����� ���������� ���
                //   E_CHANGE_MASS.
                break;

            case E_last :
                // ������ ������� �� ���������� �����������
                break;
        }

    } // for (int w = ...

}




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void fix(
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
        asteroidFixEmitEvent( i, aa, ap, as, timestep );
    }

    if ( (i < PLANET_COUNT) && presentPlanet( &ap[ i ] ) ) {
        //planetFixEmitEvent( pc[ i ] );
    }

    if ( (i < STAR_COUNT) && presentStar( &as[ i ] ) ) {
        starFixEmitEvent( i, aa, ap, as, timestep );
    }

}
