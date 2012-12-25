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
* Общий класс для создания движков для "оживления" портуланов (3D-карт)
* без использования промежуточных booster-структур.
*
* @template P  Портулан, с которым работает движок.
* @template R  С какой точностью представлены числа.
*
* @see Проект "portulan" > https://github.com/savdalion/portulan
*/
template< class P, typename R >
class EngineWithoutBooster :
    public AEngine
{
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
        mPortulan( std::shared_ptr< P >( nullptr ) ),
        mTimestep( timestep ),
        mLive()
    {
        assert( (mTimestep >= 1.0)
            && "Шаг времени не может быть меньше секунды." );
    }



    inline EngineWithoutBooster( std::shared_ptr< portulan_t >  p,  R timestep ) :
        mPortulan( p ),
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
    virtual inline void incarnate( std::shared_ptr< portulan_t >  p,  real_t extentPortulan = 0 ) {
        assert( p && "Портулан не указан." );
        assert( (extentPortulan >= 0)
            && "Протяжённость портулана не может быть меньше 0." );

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
    * @return Протяжённость портулана.
    *         Метод должен вычислить протяжённость, если она ещё
    *         не указана в 'mExtent'.
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
    * Храним как weak_ptr: можем передавать портулан *нескольким* движкам.
    */
    std::weak_ptr< portulan_t >  mPortulan;

    /**
    * Время, которое проходит за 1 пульс.
    * Указывается в секундах.
    */
    real_t mTimestep;

    /**
    * Сколько пульсов прожила система.
    *
    * Если шаг времени (mTimestep) не меняется движком, время жизни =
    * = шаг * количество прожитых пульсов.
    */
    Pulse mLive;

    /**
    * Протяжённость портулана.
    *
    * @see extent()
    */
    real_t mExtent;
};




} // porte
