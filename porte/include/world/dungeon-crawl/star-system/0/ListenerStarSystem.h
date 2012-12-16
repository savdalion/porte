#pragma once

#include <portulan/portulan.h>
#include "Listener.h"
#include "../../planet/0/Listener.h"


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;


/**
* Слушатель событий движка звёздной системы.
* Включает слушателей от движков, которые могут повлиять на звёздную систему.
*
* @see #Соглашения в starsystem::Listener.
*/
class ListenerStarSystem :
    // слушаем звёздную систему
    public Listener,
    // слушаем планету
    public pep::Listener
{
public:
    /**
    * # Просто заботимся, чтобы слушатели получили это событие.
    */
    virtual void afterPulse( pns::real_t timelive, const pns::topology_t& );




    /**
    * # Звезда поглощает астероид.
    */
    virtual void afterAsteroidCollisionStar(
        pns::asteroidContent_t,  size_t ia,
        pns::starContent_t,      size_t ib,
        pns::deltaElement_t&
    );




    /**
    * # Печатаем инфо на консоль.
    */
    virtual void afterChangeCountAsteroid(
        size_t current,  int delta
    );
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "ListenerStarSystem.inl"
