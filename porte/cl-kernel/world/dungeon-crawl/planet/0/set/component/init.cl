// @require ���������� �� "include".


/**
* �������������� ���������� � ������� �������.
*   # ���������� ���������� �� ������� ����������, �������� �������� �
*     aboutPlanet_t ���������.
*/




/**
* �������������� ���������� � �������� ���� ������� �������.
*   # ��������, ��� ���������� ��������� � ��������� ���� ��� ������������.
*   # ��������� ����������� ��� ������������� �� ������� �� ������� ���������.
*/
inline void zoneComponent( __global componentCell_t c, __global const zoneComponent_t* z ) {
    /* @test
    for (uint k = 0; k < COMPONENT_CELL; ++k) {
        c[k].code  = CC_NONE;
        c[k].count = 0.0f;
    }
    return;
    */


    // # � 1-� ������ ��������� �� COMPONENT_CELL �����������.
    uint k = 0;
    for ( ; k < COMPONENT_CELL; ++k) {
        // c[k] typeof portionComponent_t
        // # ����� ���� �������� �� ����� COMPONENT_CELL �����������.
        const enum CODE_COMPONENT code = z[k].code;
        /* - @todo opimize ����� ��������. �� ������� ���� ��������
             �� �� ? ��������������.
        if (code == CC_NONE) {
            // ������ ���������; ������, ������ ������ ���
            break;
        }
        */
        
        const float count = z[k].count;
        
        // ��������� �����: �������� ���� ����������� ������ ������
        // � ��������� [0.0; 1.0]
        // @todo fine ��������, ������ ��� ��������� ������������� ��������. �������� ����.
        const bool outOfBorder =
            (code <= CC_NONE) || (code >= CC_last) ||
            (count < 0.0f)    || (count > 1.0f);
        //(*c)[k].code  = select( code,  CC_NONE, outOfBorder );
        //(*c)[k].count = select( count, 0.0f,    outOfBorder );
        if ( outOfBorder ) {
            c[k].code  = CC_NONE;
            c[k].count = 0.0f;
        } else {
            c[k].code  = code;
            c[k].count = count;
        }
    }

    // ����������� ��������
    for ( ; k < COMPONENT_CELL; ++k) {
        c[k].code  = CC_NONE;
        c[k].count = 0.0f;
    }
}




inline void coreComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    /* - �������. ��. ����.
    // # � 1-� ������ ��������� �� COMPONENT_CELL �����������.
    uint k = 0;
    for ( ; k < COMPONENT_CELL; ++k) {
        // c[n] typeof portionComponent_t
        // # ��� ������� ����� ���� �������� �� ����� COMPONENT_CELL �����������.
        const enum CODE_COMPONENT code = ap->component.core[k].code;
        if (code == CC_NONE) {
            // ������ ���������, ������ ������ ������ ���, �����������
            break;
        }
        c[k]->code  = code;
        c[k]->count = ap->component.core[k].count;
    }
    // ����������� ��������
    for ( ; k < COMPONENT_CELL; ++k) {
        c[k]->code  = CC_NONE;
        c[k]->count = 0.0f;
    }
    */
    zoneComponent( c, ap->component.core );
}




inline void mantleComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.mantle );
}




inline void crustComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.crust );
}




inline void atmosphereComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.atmosphere );
}




inline void spaceComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.space );
}






__kernel void init(
    __global const aboutPlanet_t*  ap,    // 0
    __global componentCell_t*      cc     // 1
) {
    // ������ - ���������� ������ 3D-�����
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see ����� -D ��� ��� ���������� ����  
    const uint i = icDenorm( dnc );

    
    /* test
    uint k = 0;
    for ( ; k < COMPONENT_CELL / 2; ++k) {
        cc[i][k].code  = CC_AIR;
        cc[i][k].count = 1.0f;
    }
    
    for ( ; k < COMPONENT_CELL; ++k) {
        cc[i][k].code  = CC_NONE;
        cc[i][k].count = 0.0f;
    }
    
    return;
    */
    
    
    
    // ���������� ��������� �� ����� ������� �������.
    // �������� � ��������� ���������.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;
    
    // ��������� ����
    if ( coreZone( ap, distanceByHalfSize ) ) {
        coreComponent( cc[i], ap );
        
    } else if ( mantleZone( ap, distanceByHalfSize ) ) {
        mantleComponent( cc[i], ap );

    } else if ( crustZone( ap, distanceByHalfSize ) ) {
        crustComponent( cc[i], ap );
        
    } else if ( atmosphereZone( ap, distanceByHalfSize ) ) {
        atmosphereComponent( cc[i], ap );
        
    } else {
        spaceComponent( cc[i], ap );
    }
    
}
