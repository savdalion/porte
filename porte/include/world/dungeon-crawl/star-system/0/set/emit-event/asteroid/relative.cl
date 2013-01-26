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
    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );

    if (i >= ASTEROID_COUNT) {
        //printf( "(!) Index %d / %d out of range for asteroid.\n",  i,  ASTEROID_COUNT - 1 );
        return;
    }

    if ( absentAsteroid( &aa[ i ] ) ) {
        return;
    }


    __global aboutAsteroid_t* element = &aa[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        //printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n",
        //    element->uid, ee->waldo );
    }

    /* @test
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
    */
#endif

    int w = ee->waldo;


    // �������� �� ������������
    // # ��������� � asteroidDirectEmitEvent(). �������� ��� ��������
    //   ���������� ����� �������.


    // ���� �� ������ ���������
    // ������� ���������, ���������� �������� ������� �������
    real3_t gfA = 0;

    const real3_t coordA = element->today.coord;
    const real_t massA = element->today.mass;

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
                    const real3_t coordB = ask->today.coord;
                    const real3_t dc = coordB - coordA;
                    const real3_t squareDC = dc * dc;
                    const real_t absDC = length( dc );
#ifdef __DEBUG
                    /* - ������ ������ ��� �������� ����, ����.
                    assertReal( absDC, "(!) Overfill distance between asteroid and star.\n" );
                    */
#endif
                    const bool correctAbsDC = testReal( absDC );
                    if ( correctAbsDC && (absDC > PRECISION) ) {
                        const real3_t normalDC = dc / absDC;
                        // f = (G * mB) * mA / distance ^ 2
                        // @todo optimize ������� '* massA' �� �����.
                        const real_t fgmm = fgm * massA;
                        // @todo optimize ������������ ������� ��� real3_t.
                        gfA += (real3_t)(
                            zero( normalDC.x ) ? 0 : (fgmm / squareDC.x),
                            zero( normalDC.y ) ? 0 : (fgmm / squareDC.y),
                            zero( normalDC.z ) ? 0 : (fgmm / squareDC.z)
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
    const real_t absForceA = length( gfA );
    const bool correctAbsForce = testReal( absForceA );
#ifdef __DEBUG
    if ( !correctAbsForce ) {
        /*
        printf( "Don't correct \'absForce\' for asteroid %d.\n"
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
        */
    }
    assertReal( absForceA, "(!) Overfill length E_IMPACT_FORCE for asteroid.\n" );
#endif
    if ( correctAbsForce && (w < EMITTER_EVENT_COUNT) ) {
        const eventTwo_t e = {
            // uid �������
            E_IMPACT_FORCE,
            // ������ �������� ������� - ����� �� �����
            {},
            { gfA.x, gfA.y, gfA.z,  absForceA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ���������
    const real3_t accelerationA = gfA / massA;
    const real_t absAccelerationA = absForceA / massA;  // = lengthVector( accelerationA )
    if ( (w < EMITTER_EVENT_COUNT) && (absAccelerationA > PRECISION) ) {
        const eventTwo_t e = {
            // uid �������
            E_IMPACT_ACCELERATION,
            // ������ �������� ������� - ����� �� �����
            {},
            // @todo optimize fine ������� �������� � ���� 4D-��������.
            { accelerationA.x, accelerationA.y, accelerationA.z,  absAccelerationA }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // �������� ��������
    const real3_t deltaVelocityA = accelerationA * timestep;
    const real3_t velocityA = element->today.velocity;
    const real_t absVelocityA = length( velocityA );
    const real3_t futureVelocityA = velocityA + deltaVelocityA;
    const real_t absFutureVelocityA = length( futureVelocityA );
    const real_t absDeltaVelocityA = absFutureVelocityA - absVelocityA;
    // # �������� ����� �������� �������. ������ ������ �. ��� ��. �������.
    if (absDeltaVelocityA > PRECISION) {
        // @todo optimize? ���� - ����������. ������ ��� - ������� ���������?
        // ����� ������� �� ��������� ��������
        if (w < EMITTER_EVENT_COUNT) {
            const eventTwo_t e = {
                // uid �������
                E_CHANGE_VELOCITY,
                // ������ �������� ������� - ����� �� �����
                {},
                { deltaVelocityA.x, deltaVelocityA.y, deltaVelocityA.z, absDeltaVelocityA }
            };
            element->emitterEvent.content[ w ] = e;
            ++w;
        }
    } // if ( !zero( absDeltaVelocityA ) )


    // �������� ����������
    const real3_t deltaCoordA = futureVelocityA * timestep;
    if ( (w < EMITTER_EVENT_COUNT) && (length( deltaCoordA ) > PRECISION) ) {
        const eventTwo_t e = {
            // uid �������
            E_CHANGE_COORD,
            // ������ �������� ������� - ����� �� �����
            {},
            { deltaCoordA.x, deltaCoordA.y, deltaCoordA.z }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // ��������� ������
    ee->waldo = w;

}
