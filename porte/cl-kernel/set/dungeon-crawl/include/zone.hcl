/**
* ������ �������� ���������� ���� ������� ������� �� ���������
* �����������.
*
* @see ���������� � helper.hcl.
*/



/**
* @return �������� ����� ����� ��������� � ���� ���� ������� �������.
*
* ������ ������ ������
    const float halfSize = ap->size / 2.0f;
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float distanceByHalfSize = distance / halfSize;
    if ( coreZone( ap, distanceByHalfSize ) ) {
        // ...
    }
* ��� ap - ���������� �� ��������� aboutPlanet_t,
*     nc - ��������������� ���������� (��. normalizeCoord()),
*     SCALE - ������� ������ �������� ������ ������� ����� (���������
*             � �������� ��������� ��� ������ ����).
*/
inline bool coreZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.core);
}


/**
* @return �������� ����� ����� ��������� � ���� ������ ������� �������.
*/
inline bool mantleZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.mantle);
}


/**
* @return �������� ����� ����� ��������� � ���� ���� ������� �������.
*/
inline bool crustZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.crust);
}


/**
* @return �������� ����� ����� ��������� � ���� ��������� ������� �������.
*/
inline bool atmosphereZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize <= ap->radius.atmosphere);
}


/**
* @return �������� ����� ����� ��������� ���� ��������� ������� �������.
*/
inline bool spaceZone( __global const aboutPlanet_t* ap, float distanceByHalfSize ) {
    return (distanceByHalfSize > ap->radius.atmosphere);
}





/**
* @return ����������� � �������� ���� ������� �������.
*         ����������� ������ � �������� ������ "z" ������.
*
* @param z  ����� ���������� � ���������.
*/
inline float zoneTemperature( __global const zoneTemperature_t* z, float radius, float distanceByHalfSize ) {
    /* - ������� ��������. ��. ����.
    const float radius = ap->radius.core * ap->size / 2.0f;
    const float realDistance = distanceByHalfSize * ap->size / 2.0f;
    // ���������� �� ������� ����������� ���� = 1.0;
    // ����� ������ <= 1.0 (��. ���������� � �������� ������)
    const float distanceBySurface = realDistance / radius;
    */
    const float distanceBySurface = distanceByHalfSize / radius;

    // # ��-�� ������ ���������� ����� ���������� ����� ���������� ���
    //   ������������ ����� ���������. �������, ������ clamp().
    
    // ������������� ��������
    // ��������������: Bool == 0 / 1
    // @see http://khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/isgreater.html
    const uint a = (uint)isgreater( z->map[0], z->map[1] );
    const uint b = (uint)( !a );

    return clamp( z->map[0] + (z->map[1] - z->map[0]) * (distanceBySurface - 1.0f),  z->map[a],  z->map[b] );
}
