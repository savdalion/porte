#pragma once

#include "../../configure.h"
#include "AEngine.h"
#include "Pulse.h"
#include <typelib/typelib.h>
#include <portulan/portulan.h>
#include <limits>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/date_time/local_time/local_time.hpp>


namespace porte {


/**
* ����� ����� ��� �������� ������� ��� "���������" ���������� (3D-����)
* ��� ������������� ������������� booster-��������.
*
* @template P  ��������, � ������� �������� ������.
* @template R  � ����� ��������� ������������ �����.
*
* @see ������ "portulan" > https://github.com/savdalion/portulan
*/
template< class P, typename R >
class EngineWithoutBooster :
    public AEngine
{
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< EngineWithoutBooster >  Ptr;
    typedef std::unique_ptr< EngineWithoutBooster >  UPtr;


    typedef P  portulan_t;
    typedef R  real_t;




public:
    inline EngineWithoutBooster( R timestep ) :
        mPortulan( std::shared_ptr< P >( nullptr ) ),
        mTimestep( timestep ),
        mLive()
    {
        assert( (mTimestep >= 1.0)
            && "��� ������� �� ����� ���� ������ �������." );
    }



    inline EngineWithoutBooster( std::shared_ptr< portulan_t >  p,  R timestep ) :
        mPortulan( p ),
        mTimestep( timestep )
    {
        assert( p && "����� �� ������� (�������� �� ������)." );
        assert( (mTimestep >= 1.0)
            && "��� ������� �� ����� ���� ������ �������." );
    }




    /**
    * ��������� ������� ������� � ������.
    * 
    * @param extentPortulan ������������ ���������. ���� �� �������,
    *        ����������� � ������� extent().
    */
    virtual inline void incarnate( std::shared_ptr< portulan_t >  p,  real_t extentPortulan = 0 ) {
        assert( p && "�������� �� ������." );
        assert( (extentPortulan >= 0)
            && "������������ ��������� �� ����� ���� ������ 0." );

        mPortulan = p;
        mExtent = typelib::empty( extentPortulan ) ? extent() : extentPortulan;
    }



    inline Pulse live() const {
        return mLive;
    }



    inline real_t timestep() const {
        return mTimestep;
    }



    inline void timestep( real_t timestep ) {
        mTimestep = timestep;
    }



    /**
    * @return ������������ ���������.
    *         ����� ������ ��������� ������������, ���� ��� ���
    *         �� ������� � 'mExtent'.
    */
    virtual real_t extent() = 0;



    inline std::shared_ptr< const portulan_t >  portulan() const {
        assert( !mPortulan.expired() );
        return mPortulan.lock();
    }


    inline std::shared_ptr< portulan_t >  portulan() {
        assert( !mPortulan.expired() );
        return mPortulan.lock();
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
    * ������ ��� weak_ptr: ����� ���������� �������� *����������* �������.
    */
    std::weak_ptr< portulan_t >  mPortulan;

    /**
    * �����, ������� �������� �� 1 �����.
    * ����������� � ��������.
    */
    real_t mTimestep;

    /**
    * ������� ������� ������� �������.
    *
    * ���� ��� ������� (mTimestep) �� �������� �������, ����� ����� =
    * = ��� * ���������� �������� �������.
    */
    Pulse mLive;

    /**
    * ������������ ���������.
    *
    * @see extent()
    */
    real_t mExtent;
};




} // porte
