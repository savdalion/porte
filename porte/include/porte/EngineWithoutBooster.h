#pragma once

#include "../../configure.h"
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
* @template SX, SY, SZ Размеры Portulan, с которым работает движок.
*
* @see Проект "portulan" > https://github.com/savdalion/portulan
*/
template< class P >
class EngineWithoutBooster {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< EngineWithoutBooster >  Ptr;
    typedef std::unique_ptr< EngineWithoutBooster >  UPtr;


    /**
    * Тип объекта, с которым работает движок.
    */
    typedef P  portulan_t;




public:
    inline EngineWithoutBooster( portulan_t* p ) :
        mPortulan( p )
    {
        assert( p && "Карта не указана (портулан не указан)." );
    }




    inline portulan_t const*  portulan() const {
        return mPortulan;
    }


    inline portulan_t*  portulan() {
        return mPortulan;
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
    portulan_t* mPortulan;

};




} // porte
