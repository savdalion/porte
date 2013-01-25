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

    if (i >= STAR_COUNT) {
        printf( "(!) Index %d / %d out of range for star.\n",  i,  STAR_COUNT - 1 );
        return;
    }

    if ( absentStar( &as[ i ] ) ) {
        return;
    }


    __global aboutStar_t* element = &as[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


    // �������, ������� ����� �������� �� ������
    const pointerElement_t piA = { GE_STAR, i, element->uid };

    // ������������ � ������� ����������
    // # ������������ �������. ������� ����������� (�.�. ���� ��
    //   �������� ������ STAR), ������� �� ������� ���. ����������.

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
                  && accordancePointerElement( GE_STAR, i, element->uid,  &ewe->pi )
                ) {
#ifdef __DEBUG
                    printf( "relative() Star %d collision with asteroid %d.\n", element->uid, aak->uid );
#endif
                    const pointerElement_t piB = { GE_ASTEROID, k, aak->uid };
                    if (w < EMITTER_EVENT_COUNT) {
                        const real_t kineticABefore = ewe->fReal[ 0 ];
                        const real_t kineticBBefore = ewe->fReal[ 1 ];
                        const real_t deltaKineticA  = ewe->fReal[ 2 ];
                        const real_t deltaKineticB  = ewe->fReal[ 3 ];
                        const eventTwo_t e = {
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
                        element->emitterEvent.content[ w ] = e;
                        ++w;
                    }
                    
                    // ������������ � ���������� �������� ��� ������ � ��. �������

                    // ���������� ����� �� ������� ���������
                    const real_t deltaMassBySecond = massAsteroid( aak );
                    const real_t deltaMassByTimestep = deltaMassBySecond;
                    if (w < EMITTER_EVENT_COUNT) {
                        const eventTwo_t e = {
                            // uid �������
                            E_CHANGE_MASS,
                            // ������ �������� ������� - ������ �� � ���
                            piB,
                            // # ���������� ��������� ������� ��������
                            //   ���������� ��� ���������� �������.
                            { deltaMassBySecond, deltaMassByTimestep }
                        };
                        element->emitterEvent.content[ w ] = e;
                        ++w;
                    }

                    if (w < EMITTER_EVENT_COUNT) {
                        const eventTwo_t e = {
                            // uid �������
                            E_INCREASE_MASS,
                            // ������ �������� ������� - ������ �� � ���
                            piB,
                            // # ���������� ��������� ������� ��������
                            //   ���������� ��� ���������� �������.
                            { deltaMassBySecond, deltaMassByTimestep }
                        };
                        element->emitterEvent.content[ w ] = e;
                        ++w;
                    }

                } // if ( (ewe->uid == E_COLLISION) && ...

            } // for (int we = ...

        } // for (uint k = ...

    } // ������������ � �����������


    // ��������� ������
    ee->waldo = w;

}
