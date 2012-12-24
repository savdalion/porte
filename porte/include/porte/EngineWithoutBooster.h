#pragma once

#include "../../configure.h"
#include "AEngine.h"
#include <typelib/typelib.h>
#include <portulan/portulan.h>
#include <limits>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/date_time/local_time/local_time.hpp>


namespace porte {


/**
* Общий класс для создания движков для "оживления" портуланов (3D-карт)
* без использования промежуточных booster-структур.
*
* @template P  Портулан, с которым работает движок.
* @template R  С какой точностью представлены числа.
*
* @see Проект "portulan" > https://github.com/savdalion/portulan
*/
template< class P, typename R >
class EngineWithoutBooster : public AEngine {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< EngineWithoutBooster >  Ptr;
    typedef std::unique_ptr< EngineWithoutBooster >  UPtr;


    typedef P  portulan_t;
    typedef R  real_t;




public:
    inline EngineWithoutBooster( R timestep ) :
        mPortulan( nullptr ),
        mPulselive( 0 ),
        mTimestep( timestep ),
        mTimelive( 0 )
    {
        assert( (mTimestep >= 1.0)
            && "Шаг времени не может быть меньше секунды." );
    }



    inline EngineWithoutBooster( std::unique_ptr< portulan_t >  p,  R timestep ) :
        mPortulan( std::move( p ) ),
        mTimestep( timestep )
    {
        assert( p && "Карта не указана (портулан не указан)." );
        assert( (mTimestep >= 1.0)
            && "Шаг времени не может быть меньше секунды." );
    }




    /**
    * Воплощает звёздную систему в движке.
    * 
    * @param extentPortulan Протяжённость портулана. Если не указана,
    *        вычисляется с помощью extent().
    */
    virtual inline void incarnate( std::unique_ptr< portulan_t >  p,  real_t extentPortulan = 0 ) {
        assert( p && "Портулан не указан." );
        assert( (extentPortulan >= 0)
            && "Протяжённость портулана не может быть меньше 0." );

        mPortulan = std::move( p );
        mExtent = typelib::empty( extentPortulan ) ? extent() : extentPortulan;
    }



    inline long long pulselive() const {
        return mPulselive;
    }



    inline real_t timestep() const {
        return mTimestep;
    }



    inline void timestep( real_t timestep ) {
        mTimestep = timestep;
    }



    inline double timelive() const {
        return mTimelive;
    }



    /**
    * @return Протяжённость портулана.
    *         Метод должен вычислить протяжённость, если она ещё
    *         не указана в 'mExtent'.
    */
    virtual real_t extent() = 0;



    inline portulan_t const*  portulan() const {
        return mPortulan.get();
    }


    inline portulan_t*  portulan() {
        return mPortulan.get();
    }




    /**
    * Метод реализует процесс изменения портулана (карты) движком.
    *
    * @param Кол-во пульсов (дискретных временных интервалов), которые
    *        проработает движок прежде, чем вернуть результат.
    *
    * @see operator>>()
    */
    inline void operator()( int n ) {
        assert( (n != 0) && "Вызов с n == 0 не имеет смысла." );
        assert( (n > 0) && "Движок умеет работать только с положительным пульсом. @todo ..." );

        pulse( n );
    }




    /**
    * @alias operator()( portulan, 1 )
    *
    * Кому-то может больше нравится синтаксис вида. Есть такие?
    *   engine << 1;
    */
    inline EngineWithoutBooster& operator<<( int n ) {
        ( *this )( n );
        return *this;
    }




protected:
    /**
    * Изменение карты движком.
    */
    virtual void pulse( int n ) = 0;




protected:
    /**
    * Портулан, с которым работает движок.
    */
    std::unique_ptr< portulan_t >  mPortulan;

    /**
    * Сколько пульсов прожила система.
    */
    long long mPulselive;

    /**
    * Время, которое проходит за 1 пульс.
    * Указывается в секундах.
    */
    real_t mTimestep;

    /**
    * Сколько времени прожила система.
    * Если шаг времени не меняется движком, время жизни =
    * = шаг * количество прожитых пульсов.
    *
    * @todo fine? На больших промежутках пострадает точность.
    */
    double mTimelive;

    /**
    * Протяжённость портулана.
    *
    * @see extent()
    */
    real_t mExtent;
};




} // porte
