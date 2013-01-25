// @require ���������� �� "include".


/**
* ��������� ������� �� ��������� ������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� direct().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void relative(
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    const real_t                       timestep  // 4
) {
    return;

    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );

    if (i >= ASTEROID_COUNT) {
        printf( "(!) Index %d / %d out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
        return;
    }

    if ( absentAsteroid( &aa[ i ] ) ) {
        return;
    }


    __global aboutAsteroid_t* element = &aa[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }

    printf( "Asteroid %d.\n"
        "Coord %e %e %e\n"
        "Velocity %e %e %e\n"
        "Size %e %e %e\n"
        "",
        element->uid,
        element->today.coord.x,
        element->today.coord.y,
        element->today.coord.z,
        element->today.velocity[ 0 ],
        element->today.velocity[ 1 ],
        element->today.velocity[ 2 ],
        element->today.size[ 0 ],
        element->today.size[ 1 ],
        element->today.size[ 2 ]
    );
#endif

    int w = ee->waldo;


    // �������� �� ������������
    // # ��������� � asteroidDirectEmitEvent(). �������� ��� ��������
    //   ���������� ����� �������.


    // ���� �� ������ ���������
    // ������� ���������, ���������� �������� ������� �������
    real4_t gfA = (real4_t)( 0 );

    real4_t coordA;
    convertFromBig3DValue( &coordA, element->today.coord );
    const real_t massA = massAsteroid( element );

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
                    real4_t coordB;
                    convertFromBig3DValue( &coordB, ask->today.coord);
                    const real4_t dc = coordB - coordA;
                    const real4_t squareDC = dc * dc;
                    const real_t lengthDC = lengthVectorAccurate( dc );
#ifdef __DEBUG
                    /* - ������ ������ ��� �������� ����, ����.
                    assertReal( lengthDC, "(!) Overfill distance between asteroid and star.\n" );
                    */
#endif
                    const bool correctLengthDC = testReal4( lengthDC );
                    if ( correctLengthDC && !zero( lengthDC ) ) {
                        const real4_t normalDC = dc / lengthDC;
                        // f = (G * mB) * mA / distance ^ 2
                        // @todo optimize ������� '* massA' �� �����.
                        const real_t fgmm = fgm * massA;
                        // @todo optimize ������������ ������� ��� real4_t.
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
    const real_t absForceA = lengthVectorAccurate( gfA );
    const bool correctAbsForce = testReal( absForceA );
#ifdef __DEBUG
    if ( !correctAbsForce ) {
        printf( "Asteroid %d.\n"
            "Coord %e %e %e\n"
            "Velocity %e %e %e\n"
            "Size %e %e %e\n"
            "",
            element->uid,
            element->today.coord.x,
            element->today.coord.y,
            element->today.coord.z,
            element->today.velocity[ 0 ],
            element->today.velocity[ 1 ],
            element->today.velocity[ 2 ],
            element->today.size[ 0 ],
            element->today.size[ 1 ],
            element->today.size[ 2 ]
        );
    }
    assertReal4( gfA, "(!) Overfill E_IMPACT_FORCE for asteroid.\n" );
    assertReal( absForceA, "(!) Overfill length E_IMPACT_FORCE for asteroid.\n" );
#endif
    if ( correctAbsForce && (w < EMITTER_EVENT_COUNT) ) {
        const eventTwo_t e = {
            // uid �������
            E_IMPACT_FORCE,
            // ������ �������� ������� - ����� �� �����
            {},
            { gfA.s0, gfA.s1, gfA.s2,  absForceA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ���������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t accelerationA = gfA / massA;
    const real_t absAccelerationA = absForceA / massA;  // = lengthVector( accelerationA )
    if (w < EMITTER_EVENT_COUNT) {
        const eventTwo_t e = {
            // uid �������
            E_IMPACT_ACCELERATION,
            // ������ �������� ������� - ����� �� �����
            {},
            // @todo optimize fine ������� �������� � ���� 4D-��������.
            { accelerationA.s0, accelerationA.s1, accelerationA.s2,  absAccelerationA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // �������� ��������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t deltaVelocityA = accelerationA * timestep;
    const real4_t velocityA = element->today.velocity;
    const real_t absVelocityA = lengthVector( velocityA );
    const real4_t futureVelocityA = velocityA + deltaVelocityA;
    const real_t absFutureVelocityA = lengthVector( futureVelocityA );
    const real_t absDeltaVelocityA = absFutureVelocityA - absVelocityA;
    // # �������� ����� �������� �������. ������ ������ �. ��� ��. �������.
    if ( !zero( absDeltaVelocityA ) ) {
        // @todo optimize? ���� - ����������. ������ ��� - ������� ���������?
        // ����� ������� �� ��������� ��������
        if (w < EMITTER_EVENT_COUNT) {
            const eventTwo_t e = {
                // uid �������
                E_CHANGE_VELOCITY,
                // ������ �������� ������� - ����� �� �����
                {},
                { deltaVelocityA.s0, deltaVelocityA.s1, deltaVelocityA.s2, absDeltaVelocityA }
            };
            element->emitterEvent.content[ w ] = e;
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
            element->emitterEvent.content[ w ] = e;
            ++w;
        }
    } // if ( !zero( absDeltaVelocityA ) )


    // �������� ����������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t deltaCoordA = futureVelocityA * timestep;
    if (w < EMITTER_EVENT_COUNT) {
        const eventTwo_t e = {
            // uid �������
            E_CHANGE_COORD,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaCoordA.s0, deltaCoordA.s1, deltaCoordA.s2 }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ������
    ee->waldo = w;

}
