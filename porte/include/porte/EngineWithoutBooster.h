#pragma once

#include "../../configure.h"
#include <typelib/typelib.h>
#include <portulan/portulan.h>
#include <limits>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/date_time/local_time/local_time.hpp>



namespace porte {

class EngineWithoutBooster;

}




namespace porte {


/**
* ����� ����� ��� �������� ������� ��� "���������" ���������� (3D-����)
* ��� ������������� ������������� booster-��������.
*
* @template SX, SY, SZ ������� Portulan, � ������� �������� ������.
*
* @see ������ "portulan" > https://github.com/savdalion/portulan
*/
class EngineWithoutBooster {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< EngineWithoutBooster >  Ptr;
    typedef std::unique_ptr< EngineWithoutBooster >  UPtr;


    /**
    * ��� �������, � ������� �������� ������.
    */
    typedef portulan::planet::Portulan  portulan_t;




public:
    inline EngineWithoutBooster( portulan_t* p ) :
        mPortulan( p )
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

        pulse( n );
    }




    /**
    * @alias operator()( portulan, 1 )
    *
    * ����-�� ����� ������ �������� ��������� ����. ���� �����?
    *   engine << 1;
    */
    inline EngineWithoutBooster& operator<<( int n ) {
        ( *this )( n );
        return *this;
    }




protected:
    /**
    * ��������� ����� �������.
    */
    virtual void pulse( int n ) = 0;



protected:
    /**
    * ��������, � ������� �������� ������.
    */
    portulan_t* mPortulan;

};




} // porte
