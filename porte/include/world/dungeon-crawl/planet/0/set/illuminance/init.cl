// @require ���������� �� "include".


/**
* ����� ������������ � ������� �������.
*/




// @define illuminance-utils.h
// @todo fine �����������, ������ ������ �������� ����� ������ � illuminance-utils.h.
void starIlluminancePlanet(
    __global illuminanceCell_t  u,
    const int ix, const int iy, const int iz,
    // �������
    const float  radius,
    const float  coord[ 3 ],
    const float  omega[ 3 ],
    // ������
    const __global aboutIlluminanceStar_t*  ais
);




/**
* @param coord  ���������� ������� � ������� �������.
* @param omega  ���� ������� ������� �� ���� XYZ.
* @param ai     ���������� �� ���������� ��������� �������.
*/
__kernel void init(
    __global const aboutPlanet_t*             ap,     // 0
    __global const float*                     coord,  // 1
    __global const float*                     omega,  // 2
    __global const aboutIlluminanceSource_t*  ais,    // 3
    __global illuminanceCell_t*               uc      // 4
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );


    // �������� �� ���� �������� �������
    // �������� ������������ - ������� � ����
    uc[ i ][ 0 ].star = 0.0f;

    // ������ �� ���� ���������� ���������, �������� ����� ������������
    // ���� �� ����
    const float c[ 3 ] = { coord[ 0 ],  coord[ 1 ],  coord[ 2 ] };
    const float o[ 3 ] = { omega[ 0 ],  omega[ 1 ],  omega[ 2 ] };
    for (uint k = 0; k < ILLUMINANCE_STAR_COUNT; ++k) {
        starIlluminancePlanet(
            uc[ i ],
            nc.x, nc.y, nc.z,
            // �������
            ap->radius.atmosphere, c, o,
            // ������
            &ais->star[ k ]
        );
    }
}
