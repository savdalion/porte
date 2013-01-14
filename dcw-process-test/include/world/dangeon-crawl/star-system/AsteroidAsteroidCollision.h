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
* - прямое столкновение
* Астероид 1
*   - неподвижен
* Астероид 2
*   - медленно летит навстречу астероиду 2 (скорость ~ 0.1 м/с)
*/
TEST_F( AsteroidAsteroidCollisionSST,  Asteroid1V0Asteroid1V01Direct ) {
#if 1

    // подготовка
#if 1

    auto& tac = topology()->asteroid.content;
    size_t countAsteroid = 0;

    const pns::real_t rx = 12;
    const pns::real_t ry = 15;
    const pns::real_t rz = 10;
    const typelib::VectorT< pns::real_t >  velocityA( 0.0, 0.0, 0.0 );
    const pns::real_t density = 2300.0;
    const pns::real_t mass =
        typelib::compute::geometry::ellipsoid::volume( rx, ry, rz ) * density;
    const pns::real_t temperature = -100.0;
    const pns::real_t albedo = 0.6;

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
            { mass, 0.0 },
            // size
            { rx, ry, rz },
            // coord
            { 0, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { velocityA.x,  velocityA.y,  velocityA.z },
            // density
            // # ~ Кремний 2330 кг / м3
            density,
            // temperature
            temperature,
            // albedo
            albedo,
            // meltingPoint
            // # ~ Кремний 1680 К
            1600,
            // boilingPoint
            // # ~ Кремний 2620 К
            2600,
            // heatCapacity
            // # ~ Кремний 800 Дж / (кг * м)
            800.0,
            // enthalpyFusion
            // # ~ Кремний 50.6 кДж / моль
            50000 / 28.0855,
            // enthalpyVaporization
            // # ~ Кремний 383 кДж / моль
            380000 / 28.0855,
            // # memoryEvent
            { 0, {} }
        };
        tac[ countAsteroid ] = asteroid;
        ++countAsteroid;
    }
#endif


    // астероид 2
    const typelib::VectorT< pns::real_t >  velocityB( -0.2, 0.0, 0.0 );
#if 1
    {
        const pns::uid_t uid = 2;
        // # Такой же, как 1-й.
        pns::aboutAsteroid_t asteroid = tac[ countAsteroid - 1 ];
        asteroid.uid = uid;
		asteroid.size[ 0 ] = 10;
		asteroid.size[ 1 ] = 12;
		asteroid.size[ 2 ] = 15;
        asteroid.coord[ 0 ] = rx * 2.0 + 0.5;
        asteroid.velocity[ 0 ] = velocityB.x;
        asteroid.velocity[ 1 ] = velocityB.y;
        asteroid.velocity[ 2 ] = velocityB.z;
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
    pnios::VolumeVTKVisual::option_t  o;
    o[ "extent" ] = engine()->extent();
    o[ "auto-scale-camera" ] = false;
    o[ "without-clear" ] = false;
    o[ "size-window" ] = 1000;

    pnios::VolumeVTKVisual  visual( o );
    visual << *portulan();

#endif  // /подготовка
    

    // делаем снимок мира (см. SetUp() и выше)
    const auto massStar = topology()->star.content[ 0 ].mass;
    const auto allMassAsteroid = topology()->asteroid.content[ 0 ].mass;


    // запускаем мир
    // задаём такое кол-во шагов, чтобы астероиды успели столкнуться
    engine()->timestep( SECOND );
    static const int pulse = 1;
    static const int needStep = 50;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, pulse, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат

    // скорости астероидов изменятся
    {
        EXPECT_GT(
            typelib::VectorT< pns::real_t >( tac[ 0 ].velocity ).length(),
            velocityA.length()
        );
        EXPECT_LT(
            typelib::VectorT< pns::real_t >( tac[ 1 ].velocity ).length(),
            velocityB.length()
        );
    }

    // температуры астероидов при такой скорости удара должны
    // остаться неизменными
    {
        EXPECT_DOUBLE_EQ( tac[ 0 ].temperature,  temperature );
        EXPECT_DOUBLE_EQ( tac[ 1 ].temperature,  temperature );
    }

    // астероиды остаются целыми (нет осколков)
    {
        const size_t n = pns::countAsteroid( tac, false );
        EXPECT_EQ( n,  countAsteroid );
    }


    // @todo Кинетическая энергия системы должна сохраняться.

#endif
}








/**
* - масса и размеры астероидов одинаковы
* - прямое столкновение
* Астероид 1
*   - неподвижен
* Астероид 2
*   - летит навстречу астероиду 2 со скоростью, достаточной для раскалывания
*     астероидов на части
*/
TEST_F( AsteroidAsteroidCollisionSST,  Asteroid1V0Asteroid1V100Direct ) {
#if 1

    // подготовка
#if 1

    auto& tac = topology()->asteroid.content;
    size_t countAsteroid = 0;

    const pns::real_t rx = 12;
    const pns::real_t ry = 15;
    const pns::real_t rz = 10;
    const typelib::VectorT< pns::real_t >  velocityA( 0.0, 0.0, 0.0 );
    const pns::real_t density = 2300.0;
    const pns::real_t mass =
        typelib::compute::geometry::ellipsoid::volume( rx, ry, rz ) * density;
    const pns::real_t temperature = -100.0;
    const pns::real_t albedo = 0.6;

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
            { mass, 0.0 },
            // size
            { rx, ry, rz },
            // coord
            { 0, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { velocityA.x, velocityA.y, velocityA.z },
            // density
            // # ~ Кремний 2330 кг / м3
            density,
            // temperature
            temperature,
            // albedo
            albedo,
            // meltingPoint
            // # ~ Кремний 1680 К
            1600,
            // boilingPoint
            // # ~ Кремний 2620 К
            2600,
            // heatCapacity
            // # ~ Кремний 800 Дж / (кг * м)
            800.0,
            // enthalpyFusion
            // # ~ Кремний 50.6 кДж / моль
            50000 / 28.0855,
            // enthalpyVaporization
            // # ~ Кремний 383 кДж / моль
            380000 / 28.0855,
            // # memoryEvent
            { 0, {} }
        };
        tac[ countAsteroid ] = asteroid;
        ++countAsteroid;
    }
#endif


    // астероид 2
    const typelib::VectorT< pns::real_t >  velocityB( -100.0, 0.0, 0.0 );
#if 1
    {
        const pns::uid_t uid = 2;
        // # Такой же, как 1-й.
        pns::aboutAsteroid_t asteroid = tac[ countAsteroid - 1 ];
        asteroid.uid = uid;
		asteroid.size[ 0 ] = 10;
		asteroid.size[ 1 ] = 12;
		asteroid.size[ 2 ] = 15;
        asteroid.coord[ 0 ] = std::abs( velocityB.x ) + 1.0;
        asteroid.velocity[ 0 ] = velocityB.x;
        asteroid.velocity[ 1 ] = velocityB.y;
        asteroid.velocity[ 2 ] = velocityB.z;
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
    pnios::VolumeVTKVisual::option_t  o;
    o[ "extent" ] = engine()->extent();
    o[ "auto-scale-camera" ] = false;
    o[ "without-clear" ] = false;
    o[ "size-window" ] = 1000;

    pnios::VolumeVTKVisual  visual( o );
    visual << *portulan();

#endif  // /подготовка
    

    // делаем снимок мира (см. SetUp() и выше)
    const auto massStar = topology()->star.content[ 0 ].mass;
    const auto allMassAsteroid = topology()->asteroid.content[ 0 ].mass;


    // запускаем мир
    // задаём такое кол-во шагов, чтобы астероиды успели столкнуться
    engine()->timestep( SECOND );
    static const int pulse = 1;
    static const int needStep = 2;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, pulse, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат

    // астероиды при такой скорости удара должны разбиться на осколки
    {
        const size_t n = pns::countAsteroid( tac, false );
        EXPECT_GT( n,  countAsteroid );
    }


    // @todo Кинетическая энергия системы должна сохраняться.

#endif
}



} // namespace
