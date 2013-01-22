// @require ���������� �� "include".


/**
* ��������� ������� �� ��������� ������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� direct().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




inline void asteroidRelativeEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutAsteroid_t* aai = &aa[ i ];
    int w = aai->emitterEvent.waldo;
#ifdef __DEBUG
    if ( !betweenInteger( w, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n", aai->uid, w );
    }
#endif


    // �������, ������� ����� �������� �� ��������


    // �������� �� ������������
    // # ��������� � asteroidDirectEmitEvent(). �������� ��� ��������
    //   ���������� ����� �������.


    // ���� �� ������ ���������
    // ������� ���������, ���������� �������� ������� �������
    real4_t gfA = (real4_t)( 0 );

    const real4_t coordA = (real4_t)(
        convertFromBigValue( aai->today.coord.x ),
        convertFromBigValue( aai->today.coord.y ),
        convertFromBigValue( aai->today.coord.z ),
        0
    );
    const real_t massA = massAsteroid( aai );

    // # ������������� ������� - ������ ����� ������.
    // # ���������� ���������� �������, ���� ������ �����������.

    // ���������� �� ����
    {
        for (uint k = 0;
            // # ����� ����� ��������������� ��������� "������ ������� -
            //   ������ ��� �������", �.�. �������� � ������ �������
            //   ������ - �����������. ��. ���������� � ������ ������.
            (k < STAR_COUNT) && presentStar( &as[ k ] );
            ++k
        ) {
            __global const aboutStar_t* ask = &as[ k ];
            __global const emitterEvent_t* ee = &ask->emitterEvent;
            for (int we = ee->waldo - 1; we >= 0; --we) {
                // @todo optimize �� ��������� �������� �����������.
                if (ee->content[ we ].uid == E_GRAVITY) {
                    // ����� ������� ��� ��������� �������
                    const real_t fgm = ee->content[ we ].fReal[ 0 ];
                    const real4_t coordB = (real4_t)(
                        ask->today.coord[ 0 ],
                        ask->today.coord[ 1 ],
                        ask->today.coord[ 2 ],
                        0
                    );
                    const real4_t dc = coordB - coordA;
                    const real4_t squareDC = dc * dc;
                    const real_t lengthDC = lengthVector( dc );
                    if ( !zero( lengthDC ) ) {
                        const real4_t normalDC = dc / lengthDC;
                        // f = (G * mB) * mA / distance ^ 2
                        // @todo optimize ������� '* massA' �� �����.
                        const real_t fgmm = fgm * massA;
                        gfA += (real4_t)(
                            zero( normalDC.s0 ) ? 0 : (fgmm / squareDC.s0),
                            zero( normalDC.s1 ) ? 0 : (fgmm / squareDC.s1),
                            zero( normalDC.s2 ) ? 0 : (fgmm / squareDC.s2),
                            0
                        ) * normalDC;
                        // # � ������ ������� �� 1 �������� ����� ���� ������
                        //   ���� �������������� ����.
                        break;
                    }
                }

            } // for (int we = ...

        } // for (uint k = ...

    } // ���������� �� ����



    // �������������� ����
    // @todo optimize �� ��������� �������� �����������.
    const real_t absForceA = lengthVector( gfA );
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid �������
            E_IMPACT_FORCE,
            // ������ �������� ������� - ����� �� �����
            {},
            { gfA.s0, gfA.s1, gfA.s2,  absForceA }
        };
        aai->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ���������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t accelerationA = gfA / massA;
    const real_t absAccelerationA = absForceA / massA;  // = lengthVector( accelerationA )
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid �������
            E_IMPACT_ACCELERATION,
            // ������ �������� ������� - ����� �� �����
            {},
            { accelerationA.s0, accelerationA.s1, accelerationA.s2,  absAccelerationA }
        };
        aai->emitterEvent.content[ w ] = e;
        ++w;
    }


    // �������� ��������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t deltaVelocityA = accelerationA * timestep;
    const real4_t velocityA = (real4_t)(
        aai->today.velocity[ 0 ],
        aai->today.velocity[ 1 ],
        aai->today.velocity[ 2 ],
        0
    );
    const real_t absVelocityA = lengthVector( velocityA );
    const real4_t futureVelocityA = velocityA + deltaVelocityA;
    const real_t absFutureVelocityA = lengthVector( futureVelocityA );
    const real_t absDeltaVelocityA = absFutureVelocityA - absVelocityA;
    // # �������� ����� �������� �������. ������ ������ �. ��� ��. �������.
    if ( !zero( absDeltaVelocityA ) ) {
        // @todo optimize? ���� - ����������. ������ ��� - ������� ���������?
        // ����� ������� �� ��������� ��������
        if (w < EMITTER_EVENT_COUNT) {
            eventTwo_t e = {
                // uid �������
                E_CHANGE_VELOCITY,
                // ������ �������� ������� - ����� �� �����
                {},
                { deltaVelocityA.s0, deltaVelocityA.s1, deltaVelocityA.s2, absDeltaVelocityA }
            };
            aai->emitterEvent.content[ w ] = e;
            ++w;
        }

        // ��� ���������� �������� �� ��������� � �������
        if (w < EMITTER_EVENT_COUNT) {
            eventTwo_t e = {
                // uid �������
                (absDeltaVelocityA < 0) ? E_DECREASE_VELOCITY : E_INCREASE_VELOCITY,
                // ������ �������� ������� - ����� �� �����
                {},
                { deltaVelocityA.s0, deltaVelocityA.s1, deltaVelocityA.s2,  absDeltaVelocityA }
            };
            aai->emitterEvent.content[ w ] = e;
            ++w;
        }
    } // if ( !zero( absDeltaVelocityA ) )


    // �������� ����������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t deltaCoordA = futureVelocityA * timestep;
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid �������
            E_CHANGE_COORD,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaCoordA.s0, deltaCoordA.s1, deltaCoordA.s2 }
        };
        aai->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ������
    aai->emitterEvent.waldo = w;
}








inline void starRelativeEmitEvent(
    const uint i,
    __global aboutAsteroid_t*  aa,
    __global aboutPlanet_t*    ap,
    __global aboutStar_t*      as,
    const real_t               timestep
) {
    __global aboutStar_t* asi = &as[ i ];
    int w = asi->emitterEvent.waldo;
#ifdef __DEBUG
    if ( !betweenInteger( w, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n", asi->uid, w );
    }
#endif


    // �������, ������� ����� �������� �� ������
    const pointerElement_t piA = { GE_STAR, i, asi->uid };

    // ������������ � ������� ����������
    // # ������������ �������. ������� ����������� (�.�. ���� ��
    //   �������� ������ STAR).

    // ������������ � �����������
    {
        for (uint k = 0;
            // # ����� ����� ��������������� ��������� "������ ������� -
            //   ������ ��� �������", �.�. �������� � ������ �������
            //   ������ - �����������. ��. ���������� � ������ ������.
            (k < ASTEROID_COUNT) && presentAsteroid( &aa[ k ] );
            ++k
        ) {
            __global const aboutAsteroid_t* aak = &aa[ k ];
            __global const emitterEvent_t* ee = &aak->emitterEvent;
            for (int we = ee->waldo - 1; we >= 0; --we) {
                __global const eventTwo_t* ewe = &ee->content[ we ];
                if ( (ewe->uid == E_COLLISION)
                  && accordancePointerElement( GE_STAR, i, asi->uid,  &ewe->pi )
                ) {
#ifdef __DEBUG
                    printf( "relative() Star %d collision with asteroid %d.\n", asi->uid, aak->uid );
#endif
                    const pointerElement_t piB = { GE_ASTEROID, k, aak->uid };
                    if (w < EMITTER_EVENT_COUNT) {
                        const real_t kineticABefore = ewe->fReal[ 0 ];
                        const real_t kineticBBefore = ewe->fReal[ 1 ];
                        const real_t deltaKineticA  = ewe->fReal[ 2 ];
                        const real_t deltaKineticB  = ewe->fReal[ 3 ];
                        eventTwo_t e = {
                            // uid �������
                            E_COLLISION,
                            // ������ �������� �������
                            piB,
                            {
                                // ������� ��� ������� ��� ���������, ������
                                // �������� �� �������
                                kineticBBefore, kineticABefore,
                                deltaKineticB,  deltaKineticA
                            }
                        };
                        asi->emitterEvent.content[ w ] = e;
                        ++w;
                    }
                    
                    // ������������ � ���������� �������� ��� ������ � ��. �������

                    // ���������� ����� �� ������� ���������
                    const real_t deltaMassBySecond = massAsteroid( aak );
                    const real_t deltaMassByTimestep = deltaMassBySecond;
                    if (w < EMITTER_EVENT_COUNT) {
                        eventTwo_t e = {
                            // uid �������
                            E_CHANGE_MASS,
                            // ������ �������� ������� - ������ �� � ���
                            piB,
                            // # ���������� ��������� ������� ��������
                            //   ���������� ��� ���������� �������.
                            { deltaMassBySecond, deltaMassByTimestep }
                        };
                        asi->emitterEvent.content[ w ] = e;
                        ++w;
                    }

                    if (w < EMITTER_EVENT_COUNT) {
                        eventTwo_t e = {
                            // uid �������
                            E_INCREASE_MASS,
                            // ������ �������� ������� - ������ �� � ���
                            piB,
                            // # ���������� ��������� ������� ��������
                            //   ���������� ��� ���������� �������.
                            { deltaMassBySecond, deltaMassByTimestep }
                        };
                        asi->emitterEvent.content[ w ] = e;
                        ++w;
                    }

                } // if ( (ewe->uid == E_COLLISION) && ...

            } // for (int we = ...

        } // for (uint k = ...

    } // ������������ � �����������


    // ��������� ������
    asi->emitterEvent.waldo = w;
}




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void relative(
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
        asteroidRelativeEmitEvent( i, aa, ap, as, timestep );
    }

    if ( (i < PLANET_COUNT) && presentPlanet( &ap[ i ] ) ) {
        //planetRelativeEmitEvent( pc[ i ] );
    }

    if ( (i < STAR_COUNT) && presentStar( &as[ i ] ) ) {
        starRelativeEmitEvent( i, aa, ap, as, timestep );
    }

}
