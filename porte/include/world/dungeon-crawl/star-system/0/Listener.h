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
    virtual void afterPulse( AEngine::Ptr whose ) {}




    /**
    * �������� ���������� �� �������.
    *
    * @param deltaCountAsteroid ������� ��������� � ���-�� ����������.
    */
    virtual void afterAsteroidCollisionStar(
        AEngine::Ptr whose,
        pns::asteroidContent_t,  size_t ia,
        pns::starContent_t,      size_t ib,
        const pns::deltaElement_t&
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
