#pragma once

#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;



/**
* Слушатель событий от звёзд, звёздной системы.
*
* # Здесь прописаны события, которые случаются у звезды.
* # События указаны в виде заглушек.
* # Признаком, что метод является частью слушателя Listener, служит префикс
*   'before' или 'after' в названии метода.
* # Методы событий получают всю информацию о событии.
*   Например, в виде параметров
*     ( Группа участников A, ia, Группа участников B, ib )
*     где ia, ib
* # Слушатель не может менять содержимое участников.
*/
class ListenerStar {
public:
    typedef std::shared_ptr< ListenerStar >  Ptr;


public:
    /**
    * Событие звезды "как оно есть".
    */
    virtual inline void beforeEvent(
        const pns::pointerElement_t& piA,
        const pns::eventTwo_t&
    ) {}




    // @todo Детализировать события.

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
