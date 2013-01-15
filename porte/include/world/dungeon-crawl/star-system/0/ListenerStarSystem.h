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
        const pns::starContent_t,  size_t ib
    );




    void notifyAndCompleteEventAsteroidCollisionPlanet(
        pns::asteroidContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    );




    void notifyAndCompleteEventAsteroidCollisionAsteroid(
        pns::asteroidContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    );




    void notifyAndCompleteEventAsteroidImpactForce(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    );




    void notifyAndCompleteEventAsteroidChangeCoord(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    );




    void notifyAndCompleteEventAsteroidChangeTemperature(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaTemperature
    );




    void notifyAndCompleteEventAsteroidChangeVelocity(
        pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    );




    void notifyAndCompleteEventAsteroidCrushN(
        pns::asteroidContent_t,  size_t ia,
        pns::deltaElement_t& delta,
        size_t n,
        const pns::real_t  deltaVelocity[ 3 ],
        const pns::real_t  deltaTemperature
    );




    void notifyAndCompleteEventAsteroidDestroy(
        pns::asteroidContent_t,  size_t ia,
        pns::deltaElement_t& delta
    );




    void notifyAndCompleteEventPlanetCollisionStar(
        pns::planetContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    );




    void notifyAndCompleteEventPlanetCollisionPlanet(
        pns::planetContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    );




    void notifyAndCompleteEventPlanetCollisionAsteroid(
        pns::planetContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    );




    void notifyAndCompleteEventPlanetImpactForce(
        pns::planetContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    );




    void notifyAndCompleteEventPlanetChangeCoord(
        pns::planetContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    );




    void notifyAndCompleteEventPlanetChangeVelocity(
        pns::planetContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    );




    void notifyAndCompleteEventPlanetDestroy(
        pns::planetContent_t,  size_t ia,
        pns::deltaElement_t& delta
    );




    void notifyAndCompleteEventStarCollisionStar(
        pns::starContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    );




    void notifyAndCompleteEventStarCollisionPlanet(
        pns::starContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    );




    void notifyAndCompleteEventStarCollisionAsteroid(
        pns::starContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    );




    void notifyAndCompleteEventStarImpactForce(
        pns::starContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    );




    void notifyAndCompleteEventStarChangeCoord(
        pns::starContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    );




    void notifyAndCompleteEventStarChangeMass(
        pns::starContent_t,  size_t ia,
        const pns::mass_t& deltaMass
    );




    void notifyAndCompleteEventStarChangeVelocity(
        pns::starContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    );




    void notifyAndCompleteEventStarDestroy(
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
