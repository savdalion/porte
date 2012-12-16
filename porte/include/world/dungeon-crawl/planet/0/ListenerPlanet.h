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
* ��������� ������� ������ �������.
* �������� ���������� �� �������, ������� ����� �������� �� �������.
*
* @see #���������� � starsystem::Listener.
*/
class ListenerPlanet :
    // ������� �������
    public Listener,
    // ������� ������� �������
    public pes::Listener
{
    /**
    * @virtual pes::Listener
    *
    * # ���� ��������� �����.
    */
    virtual void afterPulse( pns::real_t timelive, const pns::topology_t& );
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte






#include "ListenerPlanet.inl"
