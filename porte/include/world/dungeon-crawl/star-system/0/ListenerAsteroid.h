#pragma once

#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;



/**
* Слушатель событий от астероидов звёздной системы.
*
* @see #Соглашения у ListenerStar.
*/
class ListenerAsteroid {
public:
    typedef std::shared_ptr< ListenerAsteroid >  Ptr;


public:
    /**
    * Событие астероида "как оно есть".
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
