// @require ���������� �� "include".


/**
* ������ ������� �� ��������� ������� �������.
*
* @see �������. � starsystem::l0::EngineHybrid::pulse().
* @see ���� relative().
*
* #i NVIDIA 8800GTS �������� ������ � float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void direct(
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    const real_t                       timestep  // 4
) {
    // # ���� �������� ������� ������. ���������, ��� ���-�� ���������
    //   � ������� - ������.
    const uint i = get_global_id( 0 );


    /* @test
    if (i == 0) {
        printf( "global_0 %d  local_0 %d  groups %d   dim() %d\n",
            get_global_size( 0 ), get_local_size( 0 ), get_num_groups( 0 ),  get_work_dim() );
    }
    return;
    */


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
#endif

    int w = ee->waldo;


    // ������������ �������
    // �����������


    // �������� �� ������������
    // # ��������� ��� ������ ���� �� �������� ��������.
    //   ��. ���������� � ���� EngineHybrid::pulse().
    const real3_t coordA = element->today.coord;

    const real_t massA = element->today.mass;
    const real_t absVelocityABefore = length( element->today.velocity );
    const real_t kineticABefore =
        massA * absVelocityABefore * absVelocityABefore / 2.0f;

    // # ������������� ������� - ������ ����� ������.
    // # ���������� ���������� �������, ���� ������ �����������.

    // ������������ �� �������
    //if (w < EMITTER_EVENT_COUNT) { - ����������� � ����� ����.
    {
        // # ����������� ������, ����� �� ����������� �.
        const int MAX_EVENT_IN_ONE_LOOP = 2;
        for (uint k = 0;
            // # ����� ����� ��������������� ��������� "������ ������� -
            //   ������ ��� �������", �.�. �������� � ������ �������
            //   ������ - �����������. ��. ���������� � ������ ������.
            (k < STAR_COUNT) && presentStar( &as[ k ] )
         && (w <= (EMITTER_EVENT_COUNT - MAX_EVENT_IN_ONE_LOOP));
            ++k
        ) {
            __global const aboutStar_t* ask = &as[ k ];

            const real3_t coordB = ask->today.coord;
            // # ������ ������ ������ ���������.
            const real_t collisionDistance = ask->today.radius;
            const bool hasCollision =
                collision( coordA,  coordB,  collisionDistance );
            if ( !hasCollision ) {
                continue;
            }

#ifdef __DEBUG
            //printf( "direct() Asteroid %d collision with star %d.\n", element->uid, ask->uid );
#endif
            // @todo optimize ��� ������������ ��������� � ������ �����.
            //       ������� �� ��������� � ��������� �������. �� ����
            //       ���������...
            // ���� ����� ��������� �� �����. �������
            const real_t massB = ask->today.mass;
            const real_t absVelocityBBefore = length( ask->today.velocity );
            const real_t kineticBBefore =
                massB * absVelocityBBefore * absVelocityBBefore / 2.0f;

            // @todo optimize �������� ������� ���, ����� ����������� ��������
            //       �������� (� �����. �������) ������. ����� �� �������.
            real3_t velocityAAfter, velocityBAfter;
            speedCollisionVector(
                &velocityAAfter,  &velocityBAfter,
                massA,
                element->today.velocity,
                massB,
                ask->today.velocity,
                0.9f
            );
            const real_t absVelocityAAfter = length( velocityAAfter );
            const real_t absVelocityBAfter = length( velocityBAfter );
            const real_t kineticAAfter =
                massA * absVelocityAAfter * absVelocityAAfter / 2.0f;
            const real_t kineticBAfter =
                massB * absVelocityBAfter * absVelocityBAfter / 2.0f;
            const real_t deltaKineticA = kineticABefore - kineticAAfter;
            const real_t deltaKineticB = kineticBBefore - kineticBAfter;

            // ��� ������������ �������� ����. �������
            // #! ���������� ���� ����� ������� ������� ����������
            //    'MAX_EVENT_IN_ONE_LOOP' ����.

            // ������������ �� �������
            {
                const eventTwo_t e = {
                    // uid �������
                    E_COLLISION,
                    // ������ �������� �������
                    { GE_STAR,  k,  ask->uid },
                    {
                        kineticABefore, kineticBBefore,
                        deltaKineticA,  deltaKineticB
                    }
                };
                element->emitterEvent.content[ w ] = e;
                ++w;
            }

            // �������� ���������
            {
                const eventTwo_t e = {
                    // uid �������
                    E_DESTROY,
                    // ������ �������� ������� - �� ����� ��� ����� �������
                    {},
                    {
                        kineticABefore,
                        deltaKineticA
                    }
                };
                element->emitterEvent.content[ w ] = e;
                ++w;
            }

            // #! ���������� ���� ����� ������� ������� ����������
            //    'MAX_EVENT_IN_ONE_LOOP' ����.

        } // for (size_t k = 0 ...

    } // ������������ �� �������


    // ��������� ������
    //ee->waldo = w;

}
