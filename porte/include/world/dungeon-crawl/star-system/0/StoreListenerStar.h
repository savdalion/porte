#pragma once

#include "ListenerStar.h"
#include "../../../../porte/StoreListener.h"
#include <list>
#include <memory>


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {

class ListenerStar;


/**
* ���� �������� ������� ���������� �������.
*
* # ������ ���������� �����, ����� ������ ���������������
*   ��� ������ � StoreListener::addListener(), ����� ��������
*   ���� ��������� ��� ������ ���������.
*/
class StoreListenerStar : public StoreListener< ListenerStar > {
public:
    inline void addListenerStar(
        const std::shared_ptr< ListenerStar >  lp,
        AEngine::Ptr whose, AEngine::Ptr who
    ) {
        addListener( lp, whose, who );
    }




    inline void removeListenerStarS(
        const std::shared_ptr< ListenerStar >  lp
    ) {
        removeListener( lp );
    }
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
