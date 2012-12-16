#pragma once

#include <portulan/portulan.h>
#include "Listener.h"
#include "../../star-system/0/Listener.h"


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;


/**
* Слушатель событий движка планеты.
* Включает слушателей от движков, которые могут повлиять на планету.
*
* @see #Соглашения в starsystem::Listener.
*/
class ListenerPlanet :
    // слушаем планету
    public Listener,
    // слушаем звёздную систему
    public pes::Listener
{
    /**
    * @virtual pes::Listener
    *
    * # День сменяется ночью.
    */
    virtual void afterPulse( pns::real_t timelive, const pns::topology_t& );
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte






#include "ListenerPlanet.inl"
