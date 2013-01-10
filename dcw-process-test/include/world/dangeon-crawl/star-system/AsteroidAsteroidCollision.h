#pragma once

#include "StarSystemTest.h"


namespace {


/**
* Тестирование отработки столкновений элементов звёздной системы:
* астероиды.
*/
class AsteroidAsteroidCollisionSST : public StarSystemTest {
protected:
    inline AsteroidAsteroidCollisionSST(
    ) {
    }




    virtual inline ~AsteroidAsteroidCollisionSST() {
    }




    /**
    * @virtual StarSystemTest
    */
    virtual inline void SetUp() {
        static const pns::aboutStarSystem_t aboutStarSystem = {
            // size
            // ~ Размер ~ 10000 а.е. ~ 1.5e12 км
            { 1.5e15, 1.5e15, 1.5e15 }
        };
        topology()->aboutStarSystem = aboutStarSystem;
    }

};








/**
* - масса и размеры астероидов одинаковы
* Астероид 1
*   - неподвижен
* Астероид 2
*   - медленно летит (скорость 0.1 м/с)
*/
TEST_F( AsteroidAsteroidCollisionSST,  Asteroid1V0Asteroid1V01 ) {
#if 1

    // подготовка
#if 1

    auto& tac = topology()->asteroid.content;
    size_t countAsteroid = 0;

    const pns::real_t rx = 10;
    const pns::real_t ry = 12;
    const pns::real_t rz = 15;
    const pns::real_t density = 5000.0;
    const pns::real_t mass = 4.0 / 3.0 * M_PI * rx * ry * rz * density;

    // астероид 1
#if 1
    {
        const pns::uid_t uid = 1;
        const pns::aboutAsteroid_t asteroid = {
            // uid
            uid,
            // live
            true,
            // mass
            mass,
            // size
            { rx, ry, rz },
            // coord
            { 0, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0, 0, 0 },
            // memoryEvent
            { 0, {} }
        };
        tac[ countAsteroid ] = asteroid;
        ++countAsteroid;
    }
#endif


    // астероид 2
#if 1
    {
        const pns::uid_t uid = 2;
        const pns::aboutAsteroid_t asteroid = {
            // uid
            uid,
            // live
            true,
            // mass
            mass,
            // size
            { rx, ry, rz },
            // coord
            { -(rx * 2 + 1), 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0.1, 0, 0 },
            // memoryEvent
            { 0, {} }
        };
        tac[ countAsteroid ] = asteroid;
        ++countAsteroid;
    }
#endif

    // завершаем список астероидов пустотой
    static const pns::aboutAsteroid_t ASTEROID_END_LIST = {};
    tac[ countAsteroid ] = ASTEROID_END_LIST;


    // воплощаем
    engine()->incarnate( portulan() );


    // покажем результат
    pnios::FormVTKVisual::option_t  o;
    o[ "extent" ] = engine()->extent();
    o[ "auto-scale-camera" ] = false;
    o[ "without-clear" ] = false;
    o[ "size-window" ] = 1000;

    pnios::FormVTKVisual  visual( o );
    visual << *portulan();

#endif  // /подготовка
    

    // делаем снимок мира (см. SetUp() и выше)
    const auto massStar = topology()->star.content[ 0 ].mass;
    const auto allMassAsteroid = topology()->asteroid.content[ 0 ].mass;


    // запускаем мир
    // задаём такое кол-во шагов, чтобы астероиды успели столкнуться
    engine()->timestep( SECOND );
    static const int pulse = 1;
    static const int needStep = 60 * 1;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1000, pulse, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат
    // @todo ...

#endif
}



} // namespace
