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
*
* @todo extend Добавить слушателей 'before*()'.
*/
class Listener : public StoreListenerStarSystem {
public:
    typedef std::shared_ptr< Listener >  Ptr;


public:
    /**
    * Завершился 1 пульс звёздной системы.
    */
    virtual inline void afterPulse( AEngine::Ptr whose ) {}




    /**
    * Астероид столкнулся со звездой.
    *
    * @param deltaElement_t Счётчик изменения в кол-ве астероидов.
    *
    * # Все методы ниже вида 'after[A]Collision[B]' обозначают событие
    *   'после того, как тело A столкнулось с телом B'.
    */
    virtual inline void afterAsteroidCollisionStar(
        AEngine::Ptr whose,
        const pns::asteroidContent_t,  size_t ia,
        const pns::starContent_t,  size_t ib
    ) {}




    /**
    * У астероида изменилась температура.
    *
    * @param deltaTemperature На сколько изменилась температура.
    *
    * # Все методы ниже вида 'after[A]Change[K]' обозначают событие
    *   'после того, как у тела A ихменилась характеристика K'.
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
    * Астероид раскололся на N частей.
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
    * На астероид действует сила.
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
    * Астероид уничтожен.
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
    * На планету действует сила.
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
    * Планета уничтожена.
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
    * На звезду действует сила.
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
    * Звезда уничтожена.
    */
    virtual inline void afterStarDestroy(
        AEngine::Ptr whose,
        const pns::starContent_t,  size_t ia
    ) {}




    /**
    * Количество элементов изменилось.
    *
    * @param current Текущее кол-во.
    * @param delta   Как изменилось кол-во: плюс - увеличилось,
    *        минус - уменьшилось.
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
