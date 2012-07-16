#include "include/pragma.hcl"
#include "include/struct.hcl"
#include "include/helper.hcl"


/**
* ������� ��������������� ����������� � �������� ������.
*/
__kernel void calcHeatTransfer(
    __global float* temperature,        // 0
    __global float* workTemperature     // 1
) {
    // ������ - ���������� ������ 3D-�����
    const int x = get_global_id( 0 );
    const int y = get_global_id( 1 );
    const int z = get_global_id( 2 );

    // @see ����� -D ��� ��� ���������� ����
    const uint i = x + y * N + z * NM;

    
    // @test
    temperature[i] = 1.0f;
    

    // @todo ... 
}
