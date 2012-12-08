// @require ���������� �� "include".


/**
* ��������� ������, ����������� �� ��������� ��������� ���
* ���������� 'pulseA', � ���������� ���������.
*/
__kernel void fixA(
    __global       aboutBody_t*  b,  // 0
    __global const aboutBody_t*  wb  // 1
) {
    const uint i = get_global_id( 0 );

    // # ��� ������ �� 'wb' ��������� ��� 'b'. ��. ���������� � pulseA.cl.
    b[ i ] = wb[ i ];

}
