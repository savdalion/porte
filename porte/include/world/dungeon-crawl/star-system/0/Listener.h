#pragma once

#include "StoreListenerStarSystem.h"
#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;



/**
* ��������� ������� ������ ������� �������.
* ������ ������� ������ ����������.
*
* # ����� ��������� ��� �������, ������� ����� ������������ ������ �������
*   �������.
* # ������� ������� � ���� ��������.
* # ���������, ��� ����� �������� ������ ��������� Listener, ������ �������
*   'before' ��� 'after' � �������� ������.
* # ������ ������� �������� ��� ���������� � �������.
*   ��������, � ���� ����������
*     ( ������ ���������� A, ia, ������ ���������� B, ib )
*     ��� ia, ib
* # ��������� ����� ������ ���������� ����������, �� �� ������ ������
*   �� ���������� � ������� ���������.
* # ��� �������� / ���������� ���������� ������� ������������ ����������
*   �� portulan / ... / utils.h.
*
* #! ������ ��������� ���������� ������ ���������� *������ ����* �������.
*
* #i ������ �� ��������� ��� const, �.�. ����� ����������
*    �� StoreListenerStarSystem.
*
* @todo extend �������� ���������� 'before*()'.
*/
class Listener : public StoreListenerStarSystem {
public:
    typedef std::shared_ptr< Listener >  Ptr;


public:
    /**
    * ���������� 1 ����� ������� �������.
    */
    virtual inline void afterPulse( AEngine::Ptr whose ) {}




    /**
    * �������� ���������� �� �������.
    *
    * @param deltaElement_t ������� ��������� � ���-�� ����������.
    *
    * # ��� ������ ���� ���� 'after[A]Collision[B]' ���������� �������
    *   '����� ����, ��� ���� A ����������� � ����� B'.
    */
    virtual inline void afterAsteroidCollisionStar(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    /**
    * � ��������� ���������� �����������.
    *
    * @param deltaTemperature �� ������� ���������� �����������.
    *
    * # ��� ������ ���� ���� 'after[A]Change[K]' ���������� �������
    *   '����� ����, ��� � ���� A ���������� �������������� K'.
    */
    virtual inline void afterAsteroidChangeTemperature(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        pns::real_t  deltaTemperature
    ) {}




    virtual inline void afterAsteroidChangeVelocity(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::real_t  deltaVelocity[ 3 ]
    ) {}




    /**
    * �������� ���������� �� N ������.
    */
    virtual inline void afterAsteroidCrushN(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        pns::deltaElement_t&  delta,
        size_t n,
        const pns::real_t  deltaVelocity[ 3 ],
        pns::real_t  deltaTemperature
    ) {}




    virtual inline void afterAsteroidCollisionPlanet(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::planetContent_t,    size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterAsteroidCollisionAsteroid(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterPlanetCollisionStar(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::starContent_t,    size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterPlanetCollisionPlanet(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterPlanetCollisionAsteroid(
        AEngine::Ptr whose,
        const pns::planetContent_t,    size_t ia,
        const pns::asteroidContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterStarCollisionStar(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterStarCollisionPlanet(
        AEngine::Ptr whose,
        const pns::starContent_t,    size_t ia,
        const pns::planetContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterStarCollisionAsteroid(
        AEngine::Ptr whose,
        const pns::starContent_t,      size_t ia,
        const pns::asteroidContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    /**
    * ���������� ��������� ����������.
    *
    * @param current ������� ���-��.
    * @param delta   ��� ���������� ���-��: ���� - �����������,
    *        ����� - �����������.
    */
    virtual inline void afterChangeCountAsteroid(
        size_t current,  int delta
    ) {}




    virtual inline void afterChangeCountPlanet(
        size_t current,  int delta
    ) {}




    virtual inline void afterChangeCountStar(
        size_t current,  int delta
    ) {}

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
