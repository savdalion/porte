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
* ��������� ������� ������ ������� �������.
* �������� ���������� �� �������, ������� ����� �������� �� ������� �������.
*
* @see #���������� � starsystem::Listener.
*/
template< class E >
class ListenerStarSystem :
    // ������� ������� �������
    public Listener,
    // ������� �������
    public pep::Listener
{
protected:
    inline ListenerStarSystem() :
        mEngine( nullptr )
    {
    }




public:
    /**
    * # ������ ���������, ����� ��������� �������� ��� �������.
    */
    void notifyAndCompletePulse();




    /**
    * �������� ������������ �� �������.
    *
    * # ��� ������ ���� ���� 'notify[A]Collision[B]' ����������
    *   '���� A ����������� � ����� B'.
    * # �������� ������ �������, ��� �������� ������������� �������. ������
    *   �������� ������ ��� �������� ����������� (��������) ������� �
    *   ���������� (���������) ��� ��������������, ����� *���* ���������.
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
    * # �������� ���� �� �������.
    */
    virtual void afterChangeCountAsteroid(
        size_t current,  int delta
    );




    /**
    * # �������� ���� �� �������.
    */
    virtual void afterChangeCountPlanet(
        size_t current,  int delta
    );




    /**
    * # �������� ���� �� �������.
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
