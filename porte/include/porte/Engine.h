#pragma once

#include "../../configure.h"
#include "Portulan3DBooster.h"
#include <typelib/typelib.h>
#include <portulan/portulan.h>
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
    typedef typename Portulan3DBooster< SX, SY, SZ >::portulan_t  portulan_t;
    typedef typename porte::Portulan3DBooster< SX, SY, SZ >  pb_t;




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
    inline void operator()( int pulse ) {
        assert( (pulse != 0) && "����� � pulse == 0 �� ����� ������." );
        assert( (pulse > 0) && "������ ����� �������� ������ � ������������� �������. @todo ..." );

        // �������������� ������� ��������� ����� (���������) � ������-����������
        mPortulanBooster.toBooster();

        // ��������� ��������� ���-�� �������
        for (int q = 0; q < pulse; ++q) {
            ( *this )();
        }

        // �������������� ������-��������� � ������� ���������� ����� (���������)
        mPortulanBooster.fromBooster();
    }




    /**
    * @alias operator()( portulan, 1 )
    *
    * ����-�� ����� ������ �������� ��������� ����. ���� �����?
    *   engine << 1;
    */
    inline Engine& operator<<( int pulse ) {
        ( *this )( 1 );
        return *this;
    }




protected:
    /**
    * ��������� ����� �������.
    *
    * (!) ����� ��� ������ ���� ���������������� � ������-����������.
    */
    virtual void operator()() = 0;



protected:
    /**
    * ��������, � ������� �������� ������.
    */
    pb_t mPortulanBooster;

};




} // porte
