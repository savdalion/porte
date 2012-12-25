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
* Общий класс для создания движков для "оживления" портуланов (3D-карт).
*
* @template SX, SY, SZ Размеры Portulan, с которым работает движок.
*
* @see Проект "portulan" > https://github.com/savdalion/portulan
*/
template< size_t SX, size_t SY, size_t SZ >
class Engine {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< Engine >  Ptr;
    typedef std::unique_ptr< Engine >  UPtr;


    /**
    * Тип объекта, с которым работает движок.
    */
    typedef typename PortulanBooster< SX, SY, SZ >::portulan_t  portulan_t;
    typedef typename porte::PortulanBooster< SX, SY, SZ >       pb_t;




public:
    inline Engine( portulan_t* p ) :
        mPortulanBooster( p )
    {
        assert( p && "Карта не указана (портулан не указан)." );
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

        // синхронизируем текущее состояние карты (портулана) с бустер-структурой
        mPortulanBooster.toBooster();

        pulse( n );

        // синхронизируем бустер-структуру с текущим состоянием карты (портулана)
        mPortulanBooster.fromBooster();
    }




    /**
    * @alias operator()( portulan, 1 )
    *
    * Кому-то может больше нравится синтаксис вида. Есть такие?
    *   engine << 1;
    */
    inline Engine& operator<<( int n ) {
        ( *this )( n );
        return *this;
    }




protected:
    /**
    * Изменение карты движком.
    *
    * (!) Карта уже должна быть синхронизирована с бустер-структурой.
    */
    virtual void pulse( int n ) = 0;



protected:
    /**
    * Портулан, с которым работает движок.
    */
    pb_t mPortulanBooster;

};




} // porte
