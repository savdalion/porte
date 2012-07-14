#pragma once

#include <typelib/typelib.h>
#include <portulan/portulan.h>
#include <memory>



namespace porte {

template< size_t SX, size_t SY, size_t SZ >
class Engine;

}




namespace porte {


/**
* ����� ����� ��� �������� ������� ��� "���������" ���������� (3D-����).
*
* @template SX, SY, SZ ������� Portulan, � ������� �������� ������.
*
* @see ������ "portulan" > https://github.com/savdalion/portulan
*/
template< size_t SX, size_t SY, size_t SZ >
class Engine {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< Engine >  Ptr;
    typedef std::unique_ptr< Engine >  UPtr;


public:
    /**
    * ����� ��������� ������� ��������� ��������� (�����) �������.
    *
    * @param ���-�� ������� (���������� ��������� ����������), �������
    *        ����������� ������ ������, ��� ������� ���������.
    *
    * @see operator>>()
    */
    virtual void operator()( portulan::Portulan3D< SX, SY, SZ >&,  int pulse ) = 0;


    /**
    * @alias operator()( portulan, 1 )
    *
    * ����-�� ����� ������ �������� ��������� ����
    *   engine >> portulan;
    */
    inline Engine& operator>>( portulan::Portulan3D< SX, SY, SZ >& portulan ) {
        ( *this )( portulan, 1 );
        return *this;
    }

};




} // porte
