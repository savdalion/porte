#pragma once

#include "../../configure.h"
#include "PortulanBooster.h"
#include <typelib/typelib.h>
#include <portulan/portulan.h>
#include <limits>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/date_time/local_time/local_time.hpp>



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


    /**
    * ��� �������, � ������� �������� ������.
    */
    typedef typename PortulanBooster< SX, SY, SZ >::portulan_t  portulan_t;
    typedef typename porte::PortulanBooster< SX, SY, SZ >       pb_t;




public:
    inline Engine( portulan_t* p ) :
        mPortulanBooster( p )
    {
        assert( p && "����� �� ������� (�������� �� ������)." );
    }




    /**
    * ����� ��������� ������� ��������� ��������� (�����) �������.
    *
    * @param ���-�� ������� (���������� ��������� ����������), �������
    *        ����������� ������ ������, ��� ������� ���������.
    *
    * @see operator>>()
    */
    inline void operator()( int n ) {
        assert( (n != 0) && "����� � n == 0 �� ����� ������." );
        assert( (n > 0) && "������ ����� �������� ������ � ������������� �������. @todo ..." );

        // �������������� ������� ��������� ����� (���������) � ������-����������
        mPortulanBooster.toBooster();

        pulse( n );

        // �������������� ������-��������� � ������� ���������� ����� (���������)
        mPortulanBooster.fromBooster();
    }




    /**
    * @alias operator()( portulan, 1 )
    *
    * ����-�� ����� ������ �������� ��������� ����. ���� �����?
    *   engine << 1;
    */
    inline Engine& operator<<( int n ) {
        ( *this )( n );
        return *this;
    }




protected:
    /**
    * ��������� ����� �������.
    *
    * (!) ����� ��� ������ ���� ���������������� � ������-����������.
    */
    virtual void pulse( int n ) = 0;



protected:
    /**
    * ��������, � ������� �������� ������.
    */
    pb_t mPortulanBooster;

};




} // porte
