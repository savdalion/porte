// @require ���������� �� "include".


/**
* ����� ������������ � ������� �������.
*/




// @define illuminance-utils.h
// @todo fine �����������, ������ ������ �������� ����� ������ � illuminance-utils.h.
void starIlluminance(
    __global illuminanceCell_t  u,
    // �������
    const cl_float  radius,
    const cl_float  coord[ 3 ],
    const cl_float  omega[ 3 ]
    // ������
    const cl_float  radiusStar,
    const cl_float  coordStar[ 3 ],
    const cl_float  luminousIntensityStar
);




/**
* @param coord  ���������� ������� � ������� �������.
* @param omega  ���� ������� ������� �� ���� XYZ.
* @param ai     ���������� �� ���������� ��������� �������.
*/
__kernel void init(
    __global const aboutPlanet_t*       ap,       // 0
    __global const cl_float*            coord     // 1
    __global const cl_float*            omega     // 2
    __global const aboutIlluminance_t*  ai        // 3
    __global illuminanceCell_t*         uc        // 4
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
    // �������� ������������ - ��������� � ����
    uc[ i ][ 0 ].star = 0.0f;

    // ������ �� ���� ���������� ���������, �������� ����� ������������
    // ���� �� ����
    for (uint k = 0; k < ABOUT_ILLUMINANCE_STAR_COUNT; ++k) {
        starIlluminance(
            uc[ i ],
            // �������
            ap->radius, coord, omega,
            // ������
            ai.star[ k ].radius,
            ai.star[ k ].coord,
            ai.star[ k ].luminousIntensity
        );
    }

    // @todo ��. aboutIlluminance_t, illuminance

}
