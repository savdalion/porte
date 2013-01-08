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
    * @param deltaCountAsteroid ������� ��������� � ���-�� ����������.
    *
    * # ��� ������ ���� ���� 'after[A]Collision[B]' ���������� �������
    *   '����� ����, ��� ���� A ����������� � ����� B'.
    */
    virtual inline void afterAsteroidCollisionStar(
        AEngine::Ptr whose,
        pns::asteroidContent_t,  size_t ia,
        pns::starContent_t,      size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterAsteroidCollisionPlanet(
        AEngine::Ptr whose,
        pns::asteroidContent_t,  size_t ia,
        pns::planetContent_t,    size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterAsteroidCollisionAsteroid(
        AEngine::Ptr whose,
        pns::asteroidContent_t,  size_t ia,
        pns::asteroidContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterPlanetCollisionStar(
        AEngine::Ptr whose,
        pns::planetContent_t,  size_t ia,
        pns::starContent_t,    size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterPlanetCollisionPlanet(
        AEngine::Ptr whose,
        pns::planetContent_t,  size_t ia,
        pns::planetContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterPlanetCollisionAsteroid(
        AEngine::Ptr whose,
        pns::planetContent_t,    size_t ia,
        pns::asteroidContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterStarCollisionStar(
        AEngine::Ptr whose,
        pns::starContent_t,  size_t ia,
        pns::starContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterStarCollisionPlanet(
        AEngine::Ptr whose,
        pns::starContent_t,    size_t ia,
        pns::planetContent_t,  size_t ib,
        const pns::deltaElement_t&
    ) {}




    virtual inline void afterStarCollisionAsteroid(
        AEngine::Ptr whose,
        pns::starContent_t,      size_t ia,
        pns::asteroidContent_t,  size_t ib,
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
