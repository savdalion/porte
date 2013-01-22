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

    if ( (i > ASTEROID_COUNT) || absentAsteroid( &aa[ i ] ) ) {
        return;
    }

    __global aboutAsteroid_t* element = &aa[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Asteroid %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


    // �������� �� ������������
    // # ��������� � asteroidDirectEmitEvent(). �������� ��� ��������
    //   ���������� ����� �������.


    // ���� �� ������ ���������
    // ������� ���������, ���������� �������� ������� �������
    real4_t gfA = (real4_t)( 0 );

    const real4_t coordA = (real4_t)(
        convertFromBigValue( element->today.coord.x ),
        convertFromBigValue( element->today.coord.y ),
        convertFromBigValue( element->today.coord.z ),
        0
    );
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
        element->emitterEvent.content[ w ] = e;
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
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // �������� ��������
    // @todo optimize �� ��������� �������� ���������.
    const real4_t deltaVelocityA = accelerationA * timestep;
    const real4_t velocityA = (real4_t)(
        element->today.velocity[ 0 ],
        element->today.velocity[ 1 ],
        element->today.velocity[ 2 ],
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
        eventTwo_t e = {
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
