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
* Слушатель событий движка звёздной системы.
* Научен хранить других слушателей.
*
* # Здесь прописаны все события, которые может генерировать движок звёздной
*   системы.
* # События указаны в виде заглушек.
* # Признаком, что метод является частью слушателя Listener, служит префикс
*   'before' или 'after' в названии метода.
* # Методы событий получают всю информацию о событии.
*   Например, в виде параметров
*     ( Группа участников A, ia, Группа участников B, ib )
*     где ia, ib
* # Слушатель может менять содержимое участников, но не должен менять
*   их размещение в индексе элементов.
* # Для удаления / добавления участников следует использовать помощников
*   из portulan / ... / utils.h.
*
* #! Движки обязуются пересылать другим слушателям *только свои* события.
*
* #i Методы не объявлены как const, т.к. класс наследован
*    от StoreListenerStarSystem.
*/
class Listener : public StoreListenerStarSystem {
public:
    typedef std::shared_ptr< Listener >  Ptr;


public:
    /**
    * Завершился 1 пульс звёздной системы.
    */
    virtual void afterPulse( AEngine::Ptr whose ) {}




    /**
    * Астероид столкнулся со звездой.
    *
    * @param deltaCountAsteroid Счётчик изменения в кол-ве астероидов.
    */
    virtual void afterAsteroidCollisionStar(
        AEngine::Ptr whose,
        pns::asteroidContent_t,  size_t ia,
        pns::starContent_t,      size_t ib,
        const pns::deltaElement_t&
    ) {}




    /**
    * Количество элементов изменилось.
    *
    * @param current Текущее кол-во.
    * @param delta   Как изменилось кол-во: плюс - увеличилось,
    *        минус - уменьшилось.
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
