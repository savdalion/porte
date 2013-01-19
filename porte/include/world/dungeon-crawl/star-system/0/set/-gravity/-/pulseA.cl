// @require ���������� �� "include".


/**
* �������������� ���������� ��� � ����� ���������� (�������
* ������� �������). 
* ������������ ��������������, ���������� ����������� �� ��������� �������.
*
* @todo fine ���������� �� double. NVIDIA 8800GTS �������� ������ � float.
*/



/**
* �������� ������� B �� ������� A.
*/
inline void planetPlanetInteraction(
    __global       aboutPlanet_t*  wa,
    __global const aboutPlanet_t*  a,
    __global const aboutPlanet_t*  b,
             const float           timestep
) {
    // @todo optimize ����� ����������� ��������������. ��. ������
    //       OpenCL / NBody �� NVIDIA.

    // ����������
    const float4 r = (float4)(
        a->coord[ 0 ] - b->coord[ 0 ],
        a->coord[ 1 ] - b->coord[ 1 ],
        a->coord[ 2 ] - b->coord[ 2 ],
        0.0
    );
    const float distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    const float distance = rsqrt( distanceSquared );

    // ���� ����������
    const float forceWithoutMassA = G * b->mass / distanceSquared;
    const float force = forceWithoutMassA * a->mass;
    const float4 rn = r / distance;
    wa->force[ 0 ] += rn.x * force;
    wa->force[ 1 ] += rn.y * force;
    wa->force[ 2 ] += rn.z * force;

    // ��������
    //const float acceleration = force / a->mass;
    const float acceleration = forceWithoutMassA;
    const float velocity = acceleration * timestep;
    wa->velocity[ 0 ] += rn.x * velocity;
    wa->velocity[ 1 ] += rn.y * velocity;
    wa->velocity[ 2 ] += rn.z * velocity;
}




/**
* �������� ������ B �� ������� A.
*/
inline void planetStarInteraction(
    __global       aboutPlanet_t*  wa,
    __global const aboutPlanet_t*  a,
    __global const aboutStar_t*    b,
             const float           timestep
) {
    // @todo optimize ����� ����������� ��������������. ��. ������
    //       OpenCL / NBody �� NVIDIA.

    // ����������
    const float4 r = (float4)(
        a->coord[ 0 ] - b->coord[ 0 ],
        a->coord[ 1 ] - b->coord[ 1 ],
        a->coord[ 2 ] - b->coord[ 2 ],
        0.0
    );
    const float distanceSquared = r.x * r.x + r.y * r.y + r.z * r.z;
    const float invertDistance = rsqrt( distanceSquared );

    // ���� ����������
    const float forceWithoutMassA = G * b->mass / distanceSquared;
    const float force = forceWithoutMassA * a->mass;
    const float4 rn = r * invertDistance;
    wa->force[ 0 ] += rn.x * force;
    wa->force[ 1 ] += rn.y * force;
    wa->force[ 2 ] += rn.z * force;

    // ��������
    //const float acceleration = force / a->mass;
    const float acceleration = forceWithoutMassA;
    const float velocity = acceleration * timestep;
    wa->velocity[ 0 ] += rn.x * velocity;
    wa->velocity[ 1 ] += rn.y * velocity;
    wa->velocity[ 2 ] += rn.z * velocity;
}




/**
* ������������ �������������� ������� � ������ ������� �������.
*/
inline void planetInteraction(
    __global       aboutPlanet_t*      wap,
    __global const aboutPlanet_t*      p,
             const uint                i,
    __global const aboutStarSystem_t*  ass,
    __global const aboutBody_t*        ab,
             const float               timestep
) {
    // ������������ ���� � ��������
    // �������� ��� �������� �� ������ ��������
    // @todo optimize ����������� �������� � __local-����������.
    wap->force[ 0 ] =
        wap->force[ 1 ] =
        wap->force[ 2 ] = 0.0;

    for (uint k = 0; k < BODY_COUNT; ++k) {
        if (k == i) {
            continue;
        }

        // ���� ����������, ����������� �� ������� �� �������
        // ���� � �������� 'k'

        const enum GROUP_ELEMENT group = ab[ k ].group;
        switch ( group ) {
            case GE_PLANET:
                //planetPlanetInteraction( wap, p, &ab[ k ].content.planet, timestep );
                break;

            case GE_STAR:
                planetStarInteraction( wap, p, &ab[ k ].content.star, timestep );
                break;

            default:
                // ������ ���� �� ��������� �� �������
                continue;
        }

    } // for (uint k = 0; k < BODY_COUNT; ++k)


    // ��������� ����� ����������
    wap->coord[ 0 ] += wap->velocity[ 0 ] * timestep;
    wap->coord[ 1 ] += wap->velocity[ 1 ] * timestep;
    wap->coord[ 2 ] += wap->velocity[ 2 ] * timestep;
}





// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void pulseA(
    __global const aboutStarSystem_t*  ass,      // 0
    __global const aboutBody_t*        ab,       // 1
    __global       aboutBody_t*        wab,      // 2
             const float               timestep  // 3
) {
    const uint i = get_global_id( 0 );
    
    // # ������������� ������ � 'wb', ����� ����� ���� �������������
    //   ���������. ��. fix*.cl.
    wab[ i ] = ab[ i ];

    const enum GROUP_ELEMENT group = ab[ i ].group;

    // # ���� ��� ���. ���. ����� ������������� � ����� �������.
    // @todo bad optimize ����������� ������� = ����������� �����������.
    switch ( group ) {
        case GE_PLANET:
            planetInteraction(
                &wab[ i ].content.planet,
                &ab[ i ].content.planet,
                i, ass, ab, timestep
            );
            break;

        case GE_STAR:
            // ...
            break;

        default:
            // �� ������������ ������ ��������
            return;
    }

}
