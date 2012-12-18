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
* ��������� ������� ������ �������.
* ������ ������� ������ ����������.
*
* @see #���������� � starsystem::Listener.
*/
class Listener : public StoreListenerPlanet {
public:
    typedef std::shared_ptr< Listener >  Ptr;


public:
    /**
    * ���������� 1 ����� �������.
    */
    virtual void afterPulse( AEngine::Ptr whose ) {}



    // @todo �������� ������� �� �������, ������� ����� ���������� �� �������
    //       �������.
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte
