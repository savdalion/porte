#pragma once

#include "Listener.h"
#include "../../star-system/0/Listener.h"
#include <portulan/portulan.h>
#include <portulan/include/world/dangeon-crawl/planet/0/set/illuminance.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = ::porte::world::dungeoncrawl::starsystem::l0;


/**
* Слушатель событий движка планеты.
* Включает слушателей от движков, которые могут повлиять на планету.
*
* @see #Соглашения в starsystem::Listener.
*/

template< class E >
class ListenerPlanet :
    // слушаем планету
    public Listener,
    // слушаем звёздную систему
    public pes::Listener
{
protected:
    inline ListenerPlanet() :
        mEngine( nullptr )
    {
    }




public:
    /**
    * # Просто заботимся, чтобы слушатели получили это событие.
    */
    void completePulse();




    /**
    * Переносит неокоторые характеристики звёздной системы в область планеты.
    */
    void updatePlanetFromStarSystem( const pns::topology_t& );




    // События от звёздной системы //


    /**
    * @virtual pes::Listener
    *
    * # День сменяется ночью.
    */
    virtual void afterPulse( AEngine::Ptr whose );




protected:
    E* mEngine;
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte






#include "ListenerPlanet.inl"
