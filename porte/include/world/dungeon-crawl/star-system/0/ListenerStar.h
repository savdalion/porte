#pragma once

#include <portulan/portulan.h>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;



/**
* ��������� ������� �� ����, ������� �������.
*
* # ����� ��������� �������, ������� ��������� � ������.
* # ������� ������� � ���� ��������.
* # ���������, ��� ����� �������� ������ ��������� Listener, ������ �������
*   'before' ��� 'after' � �������� ������.
* # ������ ������� �������� ��� ���������� � �������.
*   ��������, � ���� ����������
*     ( ������ ���������� A, ia, ������ ���������� B, ib )
*     ��� ia, ib
* # ��������� �� ����� ������ ���������� ����������.
*/
class ListenerStar {
public:
    typedef std::shared_ptr< ListenerStar >  Ptr;


public:
    /**
    * ������� ������ "��� ��� ����".
    */
    virtual inline void beforeEvent(
        const pns::pointerElement_t& piA,
        const pns::eventTwo_t&
    ) {}




    // @todo �������������� �������.

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
