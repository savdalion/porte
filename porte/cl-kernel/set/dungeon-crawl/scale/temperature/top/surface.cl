// @require ���������� �� "include".


/**
* �������� ����������� �� ����������� �������.
*
* # �������������� ���� ������� init.cl.
*/



/**
* �������������� ����������� �� ����������� ����������� ����.
*   # ��������, ��� ���������� ��������� � ��������� ���� ��� ������������.
*/
inline void crustTemperature( __global temperatureCell_t t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    // # ����� ��������� ��������� � ������� �����������, �����������
    //   �������� � ������ � ���-��� ����� = 1.

    /* - ������� ��������. ��. ����.
    const float radius = ap->radius.core * ap->size / 2.0f;
    const float realDistance = distanceByHalfSize * ap->size / 2.0f;
    // ���������� �� ������� ����������� ���� = 1.0;
    // ����� ������ <= 1.0 (��. ���������� � �������� ������)
    const float distanceBySurface = realDistance / radius;
    */
    const float distanceBySurface = distanceByHalfSize / ap->radius.crust;

    // @test
    t[0].average = -11111.0f;
}




/**
* �������������� ����������� �� ����������� � ����������� ���� ���� ���������.
*   # ��������, ��� ���������� ��������� � ��������� ���� ��� ������������.
*/
inline void atmosphereTemperature( __global temperatureCell_t t, __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    // @test
    t[0].average = -22222.0f;
}






__kernel void surface(
    __global const aboutPlanet_t*  ap,    // 0
    __global temperatureCell_t*    tc     // 1
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );


    // ����������� ��������� �� ����� ������� �������.
    // ����������� ��������� ������� �������� ��������.
    // �������� � ��������� ���������.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    // �������� ������ � ������������ �������
    // # ������� ������������� � ����������� ������ ������������ �����
    //   ����� ������: ������������ � ����������� � ��� ����.
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ) {
        crustTemperature( tc[i], ap, distanceByHalfSize );

    } else if ( interiorAtmosphereZone( ap, distanceByHalfSize ) ) {
        atmosphereTemperature( tc[i], ap, distanceByHalfSize );

    } else {
        // ����������� ��������� ��� ���������
    }


    // @test
    //tc[i].average = (distanceByHalfSize <= ap->radius.core) ? 1.0f : 2.0f;
    //tc[i].average = tc[i].dispersion = tc[i].rate = -12345.0f;
    
}
