// @require ���������� �� "include".


/**
* ������ ��� ������ � ���������� � ��� ����������.
*
* @see ���������� � helper.hcl.
*/



/**
* ��������� ��������� "el" (���� ��. ���������!), ��������� ���������
* ��. ��������� �� �������� ����������.
*/
inline void landscape(
    __global       elementLandscape_t*             el,
    __global const landscapePlanet_t*              lp,
    __global const portionComponent_t*             cc,
    __global const behaviourTemperature_t*         tc,
    __global const behaviourSurfaceTemperature_t*  stc,
    __global const behaviourRainfall_t*            rc,
    __global const behaviourDrainage_t*            dc,
    uint2*                                         rstate
) {
    el->code = (enum CODE_ELEMENT_LANDSCAPE)intDiapasonRandom(
        rstate,  (int2)( 1, CEL_last - 1 )
    );

    // @todo optimize ��� �������������.
    aboutElementLandscape_t about = {};
    // @todo �������� ��������� ���������� �������� ��������� � ������.
    about.partially = D_NONE;

    // @see portulan / landscape.h
    switch ( el->code ) {
        case CEL_FLAT:
            // ��� ����������
            break;

        case CEL_MOUNTAIN:
            about.mountain.height = floatDeviationRandom(
                rstate, lp->mountain.height, lp->deviationMountain
            );
            about.mountain.slope = floatDeviationRandom(
                rstate, lp->mountain.slope,  lp->deviationMountain
            );
            break;

        case CEL_BASIN:
            // @todo ...
            break;

        case CEL_RANGE:
            // @todo ...
            break;

        case CEL_RAVINE:
            // @todo ...
            break;

        case CEL_LAKE:
            // @todo ...
            break;

        case CEL_RIVER:
            // @todo ...
            break;

        case CEL_CASCADE:
            // @todo ...
            break;

        case CEL_ROAD:
            // @todo ...
            break;

        case CEL_CAVE:
            break;

        default:
            // ��� �������������
            break;
    }

    el->about = about;
    el->count = dice( rstate, 1, 100 );
    el->scattering = floatDice( rstate, 1, 1.0f );

}
