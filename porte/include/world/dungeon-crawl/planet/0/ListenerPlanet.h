#pragma once

#include "Listener.h"
#include "../../star-system/0/Listener.h"
#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace planet {
                namespace l0 {


namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = ::porte::world::dungeoncrawl::starsystem::l0;


/**
* ��������� ������� ������ �������.
* �������� ���������� �� �������, ������� ����� �������� �� �������.
*
* @see #���������� � starsystem::Listener.
*/

template< class E >
class ListenerPlanet :
    // ������� �������
    public Listener,
    // ������� ������� �������
    public pes::Listener
{
protected:
    inline ListenerPlanet() :
        engine( nullptr )
    {
    }




public:
    /**
    * # ������ ���������, ����� ��������� �������� ��� �������.
    */
    void notifyAfterPulse();




    // ������� �� ������� ������� //


    /**
    * @virtual pes::Listener
    *
    * # ���� ��������� �����.
    */
    virtual void afterPulse( AEngine::Ptr whose );




protected:
    E* engine;
};


                } // l0
            } // planet
        } // dungeoncrawl
    } // world
} // porte






#include "ListenerPlanet.inl"
