#pragma once

#include "StoreListenerPlanet.h"
#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;


/**
* Слушатель событий движка планеты.
* Научен хранить других слушателей.
*
* @see #Соглашения в starsystem::Listener.
*/
class Listener : public StoreListenerPlanet {
public:
    typedef std::shared_ptr< Listener >  Ptr;


    // @todo Добавить события.

    // @todo Добавить события на планете, которые могут отразиться на звёздной
    //       системе.
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
