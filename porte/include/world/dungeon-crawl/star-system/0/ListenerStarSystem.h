#pragma once

#include "Listener.h"
#include "../../planet/0/Listener.h"
#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;


/**
* Слушатель событий движка звёздной системы.
* Включает слушателей от движков, которые могут повлиять на звёздную систему.
*
* @see #Соглашения в starsystem::Listener.
*/
template< class E >
class ListenerStarSystem :
    // слушаем звёздную систему
    public Listener,
    // слушаем планету
    public pep::Listener
{
protected:
    inline ListenerStarSystem() :
        mEngine( nullptr )
    {
    }




public:
    /**
    * # Просто заботимся, чтобы слушатели получили это событие.
    */
    void notifyAfterPulse();




    /**
    * # Звезда поглощает астероид.
    */
    void notifyAfterAsteroidCollisionStar(
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




protected:
    E* mEngine;
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "ListenerStarSystem.inl"
