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
    *
    * @param ��������� ������� �������.
    */
    virtual void afterPulse( pns::real_t timelive, const pns::topology_t& ) {}




    /**
    * �������� ���������� �� �������.
    *
    * @param deltaCountAsteroid ������� ��������� � ���-�� ����������.
    */
    virtual void afterAsteroidCollisionStar(
        pns::asteroidContent_t,  size_t ia,
        pns::starContent_t,      size_t ib,
        pns::deltaElement_t&
    ) {}




    /**
    * ���������� ��������� ����������.
    *
    * @param current ������� ���-��.
    * @param delta   ��� ���������� ���-��: ���� - �����������,
    *        ����� - �����������.
    */
    virtual void afterChangeCountAsteroid(
        size_t current,  int delta
    ) {}

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
