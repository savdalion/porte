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
    void notifyAndCompletePulse();




    /**
    * Астероид сталкивается со звездой.
    *
    * # Все методы ниже вида 'notify[A]Collision[B]' обозначают
    *   'тело A столкнулось с телом B'.
    * # Меняется только элемент, для которого зафиксировано событие. Второй
    *   участник должен был получить аналогичное (обратное) событие и
    *   отработает (отработал) его самостоятельно, меняя *своё* состояние.
    */
    void notifyAndCompleteEventAsteroidCollisionStar(
        pns::asteroidContent_t,  size_t ia,
        pns::starContent_t,      size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventAsteroidCollisionPlanet(
        pns::asteroidContent_t,  size_t ia,
        pns::planetContent_t,    size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventAsteroidCollisionAsteroid(
        pns::asteroidContent_t,  size_t ia,
        pns::asteroidContent_t,  size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventPlanetCollisionStar(
        pns::planetContent_t,  size_t ia,
        pns::starContent_t,    size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventPlanetCollisionPlanet(
        pns::planetContent_t,  size_t ia,
        pns::planetContent_t,    size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventPlanetCollisionAsteroid(
        pns::planetContent_t,    size_t ia,
        pns::asteroidContent_t,  size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventStarCollisionStar(
        pns::starContent_t,  size_t ia,
        pns::starContent_t,  size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventStarCollisionPlanet(
        pns::starContent_t,    size_t ia,
        pns::planetContent_t,  size_t ib,
        pns::deltaElement_t&
    );




    void notifyAndCompleteEventStarCollisionAsteroid(
        pns::starContent_t,      size_t ia,
        pns::asteroidContent_t,  size_t ib,
        pns::deltaElement_t&
    );




    /**
    * # Печатаем инфо на консоль.
    */
    virtual void afterChangeCountAsteroid(
        size_t current,  int delta
    );




    /**
    * # Печатаем инфо на консоль.
    */
    virtual void afterChangeCountPlanet(
        size_t current,  int delta
    );




    /**
    * # Печатаем инфо на консоль.
    */
    virtual void afterChangeCountStar(
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
