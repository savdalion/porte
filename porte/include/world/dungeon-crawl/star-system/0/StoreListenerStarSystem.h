#pragma once

#include "Listener.h"
#include "../../../../porte/StoreListener.h"
#include <list>
#include <memory>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

class Listener;


/**
* Учит родителя хранить слушателей событий.
*
* # Завели конкретный класс, чтобы решить неоднозначности
*   при работе с StoreListener::addListener().
*/
class StoreListenerStarSystem : public StoreListener< Listener > {
public:
    inline void addListenerStarSystem(
        const std::shared_ptr< Listener >  lp,
        AEngine::Ptr whose, AEngine::Ptr who
    ) {
        addListener( lp, whose, who );
    }




    inline void removeListenerStarSystem( const std::shared_ptr< Listener >  lp ) {
        removeListener( lp );
    }
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
