#pragma once

#include <typelib/typelib.h>
#include <portulan/portulan.h>
#include <memory>



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


public:
    /**
    * Метод реализует процесс изменения портулана (карты) движком.
    *
    * @param Кол-во пульсов (дискретных временных интервалов), которые
    *        проработает движок прежде, чем вернуть результат.
    *
    * @see operator>>()
    */
    virtual void operator()( portulan::Portulan3D< SX, SY, SZ >&,  int pulse ) = 0;


    /**
    * @alias operator()( portulan, 1 )
    *
    * Кому-то может больше нравится синтаксис вида
    *   engine >> portulan;
    */
    inline Engine& operator>>( portulan::Portulan3D< SX, SY, SZ >& portulan ) {
        ( *this )( portulan, 1 );
        return *this;
    }

};




} // porte
