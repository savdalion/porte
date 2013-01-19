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
    void completePulse();




    /**
    * �������� ������������ �� �������.
    *
    * # ��� ������ ���� ���� 'notify[A]Collision[B]' ����������
    *   '���� A ����������� � ����� B'.
    * # �������� ������ �������, ��� �������� ������������� �������. ������
    *   �������� ������ ��� �������� ����������� (��������) ������� �
    *   ���������� (���������) ��� ��������������, ����� *���* ���������.
    */
    void completeEventAsteroidCollisionStar(
        pns::asteroidContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    );




    void completeEventAsteroidCollisionPlanet(
        pns::asteroidContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    );




    void completeEventAsteroidCollisionAsteroid(
        pns::asteroidContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    );




    void completeEventAsteroidImpactForce(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    );




    void completeEventAsteroidChangeCoord(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    );




    void completeEventAsteroidChangeTemperature(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaTemperature
    );




    void completeEventAsteroidChangeVelocity(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    );




    void completeEventAsteroidCrushN(
        pns::asteroidContent_t,  size_t ia,
        pns::deltaElement_t& delta,
        size_t n,
        const pns::real_t  deltaVelocity[ 3 ],
        const pns::real_t  deltaTemperature
    );




    void completeEventAsteroidDestroy(
        pns::asteroidContent_t,  size_t ia,
        pns::deltaElement_t& delta
    );




    void completeEventPlanetCollisionStar(
        pns::planetContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    );




    void completeEventPlanetCollisionPlanet(
        pns::planetContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    );




    void completeEventPlanetCollisionAsteroid(
        pns::planetContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    );




    void completeEventPlanetImpactForce(
        pns::planetContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    );




    void completeEventPlanetChangeCoord(
        pns::planetContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    );




    void completeEventPlanetChangeVelocity(
        pns::planetContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    );




    void completeEventPlanetDestroy(
        pns::planetContent_t,  size_t ia,
        pns::deltaElement_t& delta
    );




    void completeEventStarCollisionStar(
        pns::starContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    );




    void completeEventStarCollisionPlanet(
        pns::starContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    );




    void completeEventStarCollisionAsteroid(
        pns::starContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    );




    void completeEventStarImpactForce(
        pns::starContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    );




    void completeEventStarChangeCoord(
        pns::starContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    );




    void completeEventStarChangeMass(
        pns::starContent_t,  size_t ia,
        const pns::mass_t& deltaMass
    );




    void completeEventStarChangeVelocity(
        pns::starContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    );




    void completeEventStarDestroy(
        pns::starContent_t,  size_t ia,
        pns::deltaElement_t& delta
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
