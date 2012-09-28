// @require ���������� �� "include".


/**
* ����� ����������� �� ����������� �������.
*/



/**
* �������������� ����������� �� ����������� ����������� ����.
*   # ��������, ��� ���������� ��������� � ��������� ���� ��� ������������.
*/
inline void exteriorCrustTemperature( __global surfaceTemperatureCell_t t, __global const aboutPlanet_t* ap, const int4 nc ) {
    // # ����� ��������� ��������� � ������� �����������, �����������
    //   �������� � ������ � ���-��� ����� = 1.
    
    // ��������� ���������� ����������� ������ �� ��������
    //  1.0  �������� �����
    //  0.0  �������
    // -1.0  ����� �����
    const float ed = (float)nc.z / (float)MAX_COORD_GRID;

    // ����������� �� �����������
    const float pole = ap->surfaceTemperature.crust.pole;
    const float equator = ap->surfaceTemperature.crust.equator;
    t[0].average = equator + (pole - equator) * fabs( ed );
}




/**
* �������������� ����������� �� ����������� � ����������� ���� ���� ���������.
*   # ��������, ��� ���������� ��������� � ��������� ���� ��� ������������.
*/
inline void interiorAtmosphereTemperature( __global surfaceTemperatureCell_t t, __global const aboutPlanet_t* ap, const int4 nc ) {
    // @see exteriorCrustTemperature()
    const float ed = (float)nc.z / (float)MAX_COORD_GRID;
    const float pole = ap->surfaceTemperature.crust.pole;
    const float equator = ap->surfaceTemperature.crust.equator;
    t[0].average = equator + (pole - equator) * fabs( ed );
}






__kernel void init(
    __global const aboutPlanet_t*       ap,    // 0
    __global surfaceTemperatureCell_t*  stc    // 1
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );


    // �������� � ��������� ���������.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    // �������� ������ � ������������ �������
    // # ������� ������������� � ����������� ������ ������������ �����
    //   ����� ������: ������������ � ����������� � ��� ����.
    //   �������: �.�. ����� ����� � ������� ���������� �������� ����������� �
    //   ���������� ������ ��������� ��� ��������� � ������� � "temperature".
    if ( exteriorCrustZone( ap, distanceByHalfSize ) ) {
        exteriorCrustTemperature( stc[i], ap, nc );

    } else if ( interiorAtmosphereZone( ap, distanceByHalfSize ) ) {
        interiorAtmosphereTemperature( stc[i], ap, nc );

    } else {
        // ��������� ����������� ������ ���������� ����
        stc[i][0].average = 0.0f;
    }

}
