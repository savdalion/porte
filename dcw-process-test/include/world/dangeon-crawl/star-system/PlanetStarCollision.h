#pragma once

#include "StarSystemTest.h"


namespace {


/**
* Тестирование отработки столкновений элементов звёздной системы:
* планеты и звезды.
*/
class PlanetStarCollisionSST : public StarSystemTest {
protected:
    inline PlanetStarCollisionSST(
    ) {
    }




    virtual inline ~PlanetStarCollisionSST() {
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


        // # Звезда - центр координат.
        auto& tsc = topology()->star.content;
        size_t countStar = 0;

        // Звезда
        // @source Солнце > http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
#if 1
        {
            static const pns::uid_t uid = 1;
            static const pns::aboutStar_t star = {
                // uid
                uid,
                // live
                true,
                // mass
                { 1.9891e30, 0.0 },
                // radius
                6.9551e8,
                // kernelTemperature,
                13.5e6,
                // surfaceTemperature,
                5780 + typelib::constant::physics::CK,
                // coord
                { 0, 0, 0 },
                // rotation
                { 0, 0, 0 },
                // force
                { 0, 0, 0 },
                0,
                // velocity
                { 0, 0, 0 }
            };
            tsc[ countStar ] = star;
            ++countStar;
        }
#endif

        // завершаем список звёзд пустотой
        static const pns::aboutStar_t STAR_END_LIST = {};
        tsc[ countStar ] = STAR_END_LIST;
    }




    inline pns::real_t luminosityStar() const {
        const auto& tsc = topology()->star.content;
        return typelib::compute::physics::luminosity(
            tsc[ 0 ].radius,  tsc[ 0 ].surfaceTemperature
        );
    }
};








TEST_F( PlanetStarCollisionSST,  Planet1Star1 ) {
#if 1

    // подготовка
#if 1

    // планета
    auto& tpc = topology()->planet.content;
    size_t countPlanet = 0;

    // @source Меркурий > http://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%80%D0%BA%D1%83%D1%80%D0%B8%D0%B9_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 1
    {
        const pns::uid_t uid = 1;
        const pns::real_t mass = 3.33022e23;
        const auto coord =
            typelib::CoordT< pns::real_t >( 0.4600121e11 / 20, 0, 0 );
        const auto orbitalSpeed = typelib::VectorT< pns::real_t >( 0, 0, 0 );
        const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // live
            true,
            // mass
            { mass, 0.0 },
            // radius
            2.4397e6,
            // coord
            { coord.x, coord.y, coord.z },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            0,
            // velocity
            //{ 0, 47870, 0 }
            { orbitalSpeed.x, orbitalSpeed.y, orbitalSpeed.z }
        };
        tpc[ countPlanet ] = planet;
        ++countPlanet;
    }
#endif

    // завершаем список планет пустотой
    static const pns::aboutPlanet_t PLANET_END_LIST = {};
    tpc[ countPlanet ] = PLANET_END_LIST;


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
    const auto allMassPlanet = topology()->planet.content[ 0 ].mass;


    // запускаем мир
    // задаём такое кол-во шагов, чтобы планета успела упасть на звезду
    static const int needStep = 6;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат
    EXPECT_EQ( 1,  pns::countStar( topology()->star.content, true ) );
    EXPECT_EQ( 0,  pns::countPlanet( topology()->planet.content, true ) );

    {
        const auto actual = topology()->star.content[ 0 ].mass;
        const bool gt = pns::gtMass( actual, massStar );
        EXPECT_TRUE( gt );
    }

#endif
}



} // namespace
