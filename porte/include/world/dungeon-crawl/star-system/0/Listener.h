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
        const pns::starContent_t,  size_t ib
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
        size_t n,
        const pns::real_t  deltaVelocity[ 3 ],
        pns::real_t  deltaTemperature
    ) {}




    virtual inline void afterAsteroidCollisionPlanet(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::planetContent_t,    size_t ib
    ) {}




    virtual inline void afterAsteroidCollisionAsteroid(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::asteroidContent_t,  size_t ib
    ) {}




    /**
    * �� �������� ��������� ����.
    */
    virtual inline void afterAsteroidImpactForce(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    ) {}




    virtual inline void afterAsteroidChangeCoord(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    ) {}




    virtual inline void afterAsteroidChangeVelocity(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    ) {}




    /**
    * �������� ���������.
    */
    virtual inline void afterAsteroidDestroy(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia
    ) {}




    virtual inline void afterPlanetCollisionStar(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::starContent_t,    size_t ib
    ) {}




    virtual inline void afterPlanetCollisionPlanet(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::planetContent_t,  size_t ib
    ) {}




    virtual inline void afterPlanetCollisionAsteroid(
        AEngine::Ptr whose,
        const pns::planetContent_t,    size_t ia,
        const pns::asteroidContent_t,  size_t ib
    ) {}




    /**
    * �� ������� ��������� ����.
    */
    virtual inline void afterPlanetImpactForce(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    ) {}




    virtual inline void afterPlanetChangeCoord(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    ) {}




    virtual inline void afterPlanetChangeVelocity(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    ) {}




    /**
    * ������� ����������.
    */
    virtual inline void afterPlanetDestroy(
        AEngine::Ptr whose,
        const pns::planetContent_t,  size_t ia
    ) {}




    virtual inline void afterStarCollisionStar(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    ) {}




    virtual inline void afterStarCollisionPlanet(
        AEngine::Ptr whose,
        const pns::starContent_t,    size_t ia,
        const pns::planetContent_t,  size_t ib
    ) {}




    virtual inline void afterStarCollisionAsteroid(
        AEngine::Ptr whose,
        const pns::starContent_t,      size_t ia,
        const pns::asteroidContent_t,  size_t ib
    ) {}




    /**
    * �� ������ ��������� ����.
    */
    virtual inline void afterStarImpactForce(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia,
        const pns::real_t force[ 3 ],  pns::real_t absForce
    ) {}




    virtual inline void afterStarChangeCoord(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia,
        const pns::real_t deltaCoord[ 3 ],  pns::real_t absDeltaCoord
    ) {}




    virtual inline void afterStarChangeMass(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia,
        const pns::real4_t& deltaMass
    ) {}




    virtual inline void afterStarChangeVelocity(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia,
        const pns::real_t deltaVelocity[ 3 ],  pns::real_t absDeltaVelocity
    ) {}




    /**
    * ������ ����������.
    */
    virtual inline void afterStarDestroy(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia
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
