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
