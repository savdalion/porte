#pragma once

#include "Listener.h"
#include "../../../../porte/StoreListener.h"
#include <list>
#include <memory>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {

class Listener;


/**
* Учит родителя хранить слушателей событий.
*
* # Завели конкретный класс, чтобы решить неоднозначности
*   при работе с StoreListener::addListener().
*/
class StoreListenerPlanet : public StoreListener< Listener > {
public:
    inline void addListenerPlanet( const std::shared_ptr< Listener >  lp ) {
        addListener( lp );
    }


    inline void removeListenerPlanet( const std::shared_ptr< Listener >  lp ) {
        removeListener( lp );
    }
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
