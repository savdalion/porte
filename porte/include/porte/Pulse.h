#pragma once

#include <limits>
#include <memory>



namespace porte {

class Pulse;

}




namespace porte {


/**
* ����� ����.
* ������ ��� ���������� �������� � �������� ����� �������.
*
* @see Engine
*/
class Pulse {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< Pulse >  Ptr;
    typedef std::unique_ptr< Pulse >  UPtr;


    /**
    * �������� �������� ������������� �������.
    */
    typedef long long  number_t;



public:
    explicit inline Pulse( number_t startPulse = 0,  number_t startTime = 0 ) :
        mPulselive( startPulse ),
        mTimelive( startTime )
    {
    }



    /**
    * @return ����� ����� � �������.
    */
    inline number_t pulselive() const {
        return mPulselive;
    }



    /**
    * @return ����� ����� � ��������.
    */
    inline number_t timelive() const {
        return mTimelive;
    }



    /**
    * @return ����� ����� � ������� == 0 (������ ����� �������).
    */
    inline bool first() const {
        return (mPulselive == 0);
    }



    /**
    * ��������� �� ������� ����� ������� �������� ��������.
    * ��� ���������� ���������� ������ ����� ������������ inc().
    */
    template< typename T1, typename T2 >
    inline void add( T1 deltaPulse, T2 deltaTime ) {
        mPulselive += static_cast< number_t >( deltaPulse );
        mTimelive  += static_cast< number_t >( deltaTime );
    }



    /**
    * ����������� ����� � ������� �� 1, ����� � �������� - �� 'deltaTime'.
    * �������� ������� add( 1, deltaTime).
    */
    template< typename T >
    inline void inc( T deltaTime ) {
        ++mPulselive;
        mTimelive += static_cast< number_t >( deltaTime );
    }



    /**
    * @return ������� ������� ������, ���� �� ��� ������� �� ����������
    *         ����� ���������� ������� ��������� ����������.
    *         ����������� �� number_t � ������� �������.
    */
    template< typename T >
    inline number_t pulsedelta( T timestep ) const {
        return pulsedelta( mTimelive, timestep );
    }




    /**
    * @return ������� ������� ������, ���� �� 'timestep' �� ����������
    *         ����� 'timelive' ��������� ����������.
    *         ����������� �� number_t � ������� �������.
    */
    template< typename T1, typename T2 >
    static inline number_t pulsedelta( T1 timelive, T2 timestep ) {
        return
            static_cast< number_t >( timelive ) / static_cast< number_t >( timestep );
    }




private:
    number_t  mPulselive;
    number_t  mTimelive;
};




} // porte




namespace std {

inline std::ostream& operator<<( std::ostream& out, const porte::Pulse& p ) {
    const auto tl = p.timelive();
    out << "#" << p.pulselive() << "  " <<
        typelib::time::humanDuration< porte::Pulse::number_t >( tl );
        //tl;
    return out;
}

} // std
