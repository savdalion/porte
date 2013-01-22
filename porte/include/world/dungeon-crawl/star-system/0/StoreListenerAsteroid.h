#pragma once

#include "ListenerAsteroid.h"
#include "../../../../porte/StoreListener.h"
#include <list>
#include <memory>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

class ListenerAsteroid;


/**
* Учит родителя хранить слушателей событий.
*
* @see StoreListenerStar о причине ввода отдельного класса.
*/
class StoreListenerAsteroid : public StoreListener< ListenerAsteroid > {
public:
    inline void addListenerAsteroid(
        const std::shared_ptr< ListenerAsteroid >  lp,
        AEngine::Ptr whose, AEngine::Ptr who
    ) {
        addListener( lp, whose, who );
    }




    inline void removeListenerAsteroidS(
        const std::shared_ptr< ListenerAsteroid >  lp
    ) {
        removeListener( lp );
    }
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
