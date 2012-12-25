#pragma once

#include <limits>
#include <memory>



namespace porte {

class Pulse;

}




namespace porte {


/**
* ѕульс мира.
* —лужит дл€ накоплени€ значений о прожитом миром времени.
*
* @see Engine
*/
class Pulse {
public:
    /**
    * —сылки.
    */
    typedef std::shared_ptr< Pulse >  Ptr;
    typedef std::unique_ptr< Pulse >  UPtr;


    /**
    * “очность хранени€ накапливаемых пульсов.
    */
    typedef long long  number_t;



public:
    explicit inline Pulse( number_t startPulse = 0,  number_t startTime = 0 ) :
        mPulselive( startPulse ),
        mTimelive( startTime )
    {
    }



    /**
    * @return ¬рем€ жизни в пульсах.
    */
    inline number_t pulselive() const {
        return mPulselive;
    }



    /**
    * @return ¬рем€ жизни в секундах.
    */
    inline number_t timelive() const {
        return mTimelive;
    }



    /**
    * @return ¬рем€ жизни в пульсах == 0 (первый пульс системы).
    */
    inline bool first() const {
        return (mPulselive == 0);
    }



    /**
    * ƒобавл€ет ко времени жизни системы заданные значени€.
    * ƒл€ добавлени€ единичного пульса лучше использовать inc().
    */
    template< typename T1, typename T2 >
    inline void add( T1 deltaPulse, T2 deltaTime ) {
        mPulselive += static_cast< number_t >( deltaPulse );
        mTimelive  += static_cast< number_t >( deltaTime );
    }



    /**
    * ”величивает врем€ в пульсах на 1, врем€ в секундах - на 'deltaTime'.
    * –аботает быстрее add( 1, deltaTime).
    */
    template< typename T >
    inline void inc( T deltaTime ) {
        ++mPulselive;
        mTimelive += static_cast< number_t >( deltaTime );
    }



    /**
    * @return —колько пульсов прошло, если бы шаг времени на прот€жении
    *         всего начислени€ пульсов оставалс€ посто€нным.
    *         ќкругл€етс€ до number_t в меньшую сторону.
    */
    template< typename T >
    inline number_t pulsedelta( T timestep ) const {
        return pulsedelta( mTimelive, timestep );
    }




    /**
    * @return —колько пульсов прошло, если бы 'timestep' на прот€жении
    *         всего 'timelive' оставалс€ посто€нным.
    *         ќкругл€етс€ до number_t в меньшую сторону.
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
