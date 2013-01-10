#pragma once

#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>


// Включается в "porte" для интерпретации структур C++ как OpenCL.
#undef PORTULAN_AS_OPEN_CL_STRUCT

// (!) Убираем определения define, заданные в файлах cl.h и wingdi.h, т.к.
// они совпадают с именем наших структур.
// @todo bad solution
#undef CC_NONE
#undef CL_NONE



namespace {

namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;
namespace pnios = portulan::io::world::dungeoncrawl::starsystem::l0;

namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;
namespace pniop = portulan::io::world::dungeoncrawl::planet::l0;


/**
* # Время - измерение, проходящее сквозь все портуланы. Указывается в секундах.
*/
static const double SECOND     = 1.0;
static const double MINUTE     = SECOND * 60.0;
static const double HOUR       = MINUTE * 60.0;
static const double DAY        = HOUR   * 24.0;
static const double HALF_DAY   = DAY    / 2.0;
static const double WEEK       = DAY    * 7.0;
static const double MONTH      = DAY    * 30.0;
static const double HALF_MONTH = MONTH  / 2.0;
static const double YEAR       = DAY    * 365.0;
static const double HALF_YEAR  = YEAR   / 2.0;


/**
* 'timestep' влияет на точность рассчётов (больше - ниже).
*  Может быть задействована вместе с 'PULSE'.
*/
static const double TIMESTEP = MINUTE;


/*
* # Учитываем инициал. движка звёздной системы, а именно - 'timestep'.
* 'PULSE' влияет на кол-во отрисовок (больше пульс - меньше кадров).
* Может быть задействована вместе с 'timestep'.
* Движок честно считает 'PULSE' кадров с шагом 'timestep' и только
* после - показывает картинку. Др. словами, реальное время отображаемых
* кадров = timestep * PULSE.
* @example timestep = HOUR,  PULSE = 365 * 24 - Земля будет оставаться
*          почти неподвижной, т.к. её период обращения ~ 365 дней.
* @example timestep = MINUTE,  PULSE = 60 - просчитываться модель будет
*          с точностью до 1 минуты, отображаться - каждый час своей жизни.
*/
static const int PULSE = 60;




/**
* Тестирование отработки столкновений элементов звёздной системы.
*/
class AsteroidStarCollisionStarSystemTest : public ::testing::Test {
protected:
    inline AsteroidStarCollisionStarSystemTest(
    ) :
        // Инициализируем движок звёздной системы
        // # Инициализация происходит частичная, чтобы в процессе формирования
        //   портулана звёзной системы можно было привязывать к элементам
        //   другие движки, подписывая их на события друг друга. См. wrapPlanet().
        // # Движок оборачиваем в shared_ptr, т.к. он будет отдаваться как
        //   слушатель событий другим движкам.
        mEngine( new pes::Engine( TIMESTEP ) ),
        mPortulan( new pns::Portulan() )
    {
    }




    virtual inline ~AsteroidStarCollisionStarSystemTest() {
    }




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
                1.9891e30,
                // radius
                6.9551e8,
                // temperature,
                1.5e6,
                // luminousIntensity
                3e27,
                // coord
                { 0, 0, 0 },
                // rotation
                { 0, 0, 0 },
                // force
                { 0, 0, 0 },
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




    virtual void TearDown() {
    }




    inline std::shared_ptr< pes::Engine >  engine() {
        return mEngine;
    }




    inline std::shared_ptr< pns::Portulan >  portulan() {
        return mPortulan;
    }




    inline pns::topology_t* topology() {
        return &mPortulan->topology().topology();
    }




private:
    std::shared_ptr< pes::Engine >    mEngine;
    std::shared_ptr< pns::Portulan >  mPortulan;
};








TEST_F( AsteroidStarCollisionStarSystemTest,  Asteroid1Star1 ) {
#if 1

    // астероид
    // @source http://ru.wikipedia.org/wiki/%D0%90%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    auto& tac = topology()->asteroid.content;
    size_t countAsteroid = 0;

    // для позиционирования астероидов
    static const pns::real_t asteroidOrbit = 1.49598261e11 / 10;

#if 1
    {
        const pns::uid_t uid = 1;
        const pns::real_t rx = 10e3;
        const pns::real_t ry = 20e3;
        const pns::real_t rz = 30e3;
        const pns::real_t density = 5000.0;
        const pns::real_t mass = 4.0 / 3.0 * M_PI * rx * ry * rz * density;
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
            { asteroidOrbit, 0, 0 },
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
    

    // делаем снимок мира (см. SetUp() и выше)
    const auto massStar = topology()->star.content[ 0 ].mass;
    const auto allMassAsteroid = topology()->asteroid.content[ 0 ].mass;


    // запускаем мир
    // задаём такое кол-во шагов, чтобы астероид успел упасть на звезду
    static const int needStep = 24 * 2;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат
    EXPECT_EQ( 1,  pns::countStar( topology()->star.content, true ) );
    EXPECT_EQ( 0,  pns::countAsteroid( topology()->asteroid.content, true ) );

    {
        const auto sum = massStar + allMassAsteroid;
        const auto actual = topology()->star.content[ 0 ].mass;
        EXPECT_LT( massStar, actual );
        EXPECT_DOUBLE_EQ( sum, actual );
    }

#endif
}








TEST_F( AsteroidStarCollisionStarSystemTest,  Asteroid2Star1 ) {
#if 1

    // астероиды
    // @source http://ru.wikipedia.org/wiki/%D0%90%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    auto& tac = topology()->asteroid.content;
    static const size_t N_ASTEROID = 2;
    ASSERT_LT( N_ASTEROID, pns::ASTEROID_COUNT ) <<
        "Количество астероидов превышает зарезервированный для них объём. См. ASTEROID_COUNT.";
    size_t countAsteroid = 0;

    // для позиционирования астероидов
    static const pns::real_t asteroidOrbit = 1.49598261e11 / 10;

#if 1
    std::array< pns::real_t, N_ASTEROID >  massAsteroid;
    for (size_t i = 0; i < N_ASTEROID; ++i) {
        const pns::uid_t uid = i + 1;
        const pns::real_t rx = 10e3 * (i + 1);
        const pns::real_t ry = 20e3 * (i + 1);
        const pns::real_t rz = 30e3 * (i + 1);
        const pns::real_t density = 5000.0;
        const pns::real_t mass = 4.0 / 3.0 * M_PI * rx * ry * rz * density;
        massAsteroid[ i ] = mass;
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
            { asteroidOrbit + asteroidOrbit * 0.1 * i,  0,  0 },
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

    } // for (size_t i = 0; ...
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
    

    // делаем снимок мира (см. SetUp() и выше)
    const auto massStar = topology()->star.content[ 0 ].mass;
    const pns::real_t allMassAsteroid =
        std::accumulate( massAsteroid.cbegin(), massAsteroid.cend(), 0.0 );


    // запускаем мир
    // задаём такое кол-во шагов, чтобы астероид успел упасть на звезду
    static const int needStep = 24 * 3;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат
    EXPECT_EQ( 1,  pns::countStar( topology()->star.content, true ) );
    EXPECT_EQ( 0,  pns::countAsteroid( topology()->asteroid.content, true ) );

    {
        const auto sum = massStar + allMassAsteroid;
        const auto actual = topology()->star.content[ 0 ].mass;
        //@todo ? Разрушается топология астероидов из-за обратного порядка столкновений?
        EXPECT_LT( massStar, actual );
        EXPECT_DOUBLE_EQ( sum, actual );
    }

#endif
}








TEST_F( AsteroidStarCollisionStarSystemTest,  Asteroid1000Star1 ) {
#if 1

    // астероиды
    // @source http://ru.wikipedia.org/wiki/%D0%90%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    auto& tac = topology()->asteroid.content;
    static const size_t N_ASTEROID = 1000;
    ASSERT_LT( N_ASTEROID, pns::ASTEROID_COUNT ) <<
        "Количество астероидов превышает зарезервированный для них объём. См. ASTEROID_COUNT.";
    size_t countAsteroid = 0;

    // для позиционирования астероидов
    static const pns::real_t asteroidOrbit = 1.49598261e11 / 10;

    static const size_t SEED = 12345;
    typelib::Random< size_t >  seed( 0, 1000000000, SEED );
    typelib::Random< pns::real_t >
        distance( asteroidOrbit * 0.5,  asteroidOrbit * 1.0,  seed.next() );
    typelib::Random< pns::real_t >
        angle( 0.0,  M_PI * 2.0,  seed.next() );

#if 1
    std::array< pns::real_t, N_ASTEROID >  massAsteroid;
    for (size_t i = 0; i < N_ASTEROID; ++i) {
        const pns::uid_t uid = i + 1;
        const pns::real_t rx = 10e3 * (i / 2 + 1);
        const pns::real_t ry = 20e3 * (i / 2 + 1);
        const pns::real_t rz = 30e3 * (i / 2 + 1);
        const pns::real_t density = 5000.0;
        const pns::real_t mass = 4.0 / 3.0 * M_PI * rx * ry * rz * density;
        massAsteroid[ i ] = mass;
        // расположим астероиды хаотически
        const pns::real_t d  = distance.next();
        const pns::real_t da = angle.next();
        const pns::real_t cx = d * std::sin( da );
        const pns::real_t cy = d * std::cos( da );
        const pns::real_t cz = 0;
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
            { cx, cy, cz },
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

    } // for (size_t i = 0; ...
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
    

    // делаем снимок мира (см. SetUp() и выше)
    const auto massStar = topology()->star.content[ 0 ].mass;
    const pns::real_t allMassAsteroid =
        std::accumulate( massAsteroid.cbegin(), massAsteroid.cend(), 0.0 );


    // запускаем мир
    // задаём такое кол-во шагов, чтобы астероид успел упасть на звезду
    static const int needStep = 24 * 3;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine().get() );


    // проверяем результат
    EXPECT_EQ( 1,  pns::countStar( topology()->star.content, true ) );
    EXPECT_EQ( 0,  pns::countAsteroid( topology()->asteroid.content, true ) );

    {
        const auto sum = massStar + allMassAsteroid;
        const auto actual = topology()->star.content[ 0 ].mass;
        EXPECT_LT( massStar, actual );
        EXPECT_DOUBLE_EQ( sum, actual );
    }

#endif
}



} // namespace
