#pragma once

#include "StarSystemTest.h"


namespace {


/**
* “естирование звезды в звЄздной системе.
*/
class StarSST : public StarSystemTest {
protected:
    inline StarSST(
    ) {
    }




    virtual inline ~StarSST() {
    }




    /**
    * @virtual StarSystemTest
    */
    virtual inline void SetUp() {
        static const pns::aboutStarSystem_t aboutStarSystem = {
            // size
            // ~ –азмер ~ 10000 а.е. ~ 1.5e12 км
            { 1.5e15, 1.5e15, 1.5e15 }
        };
        topology()->aboutStarSystem = aboutStarSystem;


        // добавим слушателей
        engine()->addListenerAsteroid( listenerAsteroid(), nullptr, nullptr );
        engine()->addListenerStar( listenerStar(), nullptr, nullptr );
    }

};







TEST_F( StarSST,  SingleStar ) {
#if 1

    // подготовка
    size_t countStar = 0;

    // «везда
    // @source —олнце > http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
#if 1
    {
        static const pns::uid_t uid = 1;
        const pns::real_t mass = 1.9891e30;

        const pns::real3_t coord = {};
        pns::real3_t rotation = {};

        const auto orbitalSpeed = typelib::VectorT< pns::real_t >::ZERO();
        pns::real3_t velocity = {};
        velocity.s[ 0 ] = orbitalSpeed.x;
        velocity.s[ 1 ] = orbitalSpeed.y;
        velocity.s[ 2 ] = orbitalSpeed.z;

        static const pns::characteristicStar_t today = {
            // live
            true,
            // mass
            mass,
            // radius
            6.9551e8,
            // kernelTemperature,
            13.5e6,
            // surfaceTemperature,
            5780 + typelib::constant::physics::CK,
            // coord
            coord,
            // rotation
            rotation,
            // velocity
            velocity,
            // luminosity
            0
        };
        static const pns::aboutStar_t star = {
            // uid
            uid,
            // today
            today,
            // future
            {},
            // emitterEvent
            {}
        };
        tsc[ countStar ] = star;
        ++countStar;
    }
#endif

    // завершаем список звЄзд пустотой
    static const pns::aboutStar_t STAR_END_LIST = {};
    tsc[ countStar ] = STAR_END_LIST;



    // воплощаем
    engine()->incarnate( portulan() );


    // структуры инициированы корректно?
    testAsteroid( false );


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
    const auto massStar = tsc()[ 0 ].today.mass;
    const auto allMassAsteroid = tsc()[ 0 ].today.mass;


    // запускаем мир
    // задаЄм такое кол-во шагов, чтобы астероид успел упасть на звезду
    static const int needStep = 10 * 10000;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine().get() );

    // сколько пульсов отработал движок
    const auto PL = engine()->live().pulselive();


    // провер€ем результат

    // все ли характеристики элементов остались в пределах нормы
    testAsteroid( false );
    // @todo testStar( false );

    EXPECT_EQ( 1,  pns::countStar( topology()->star.content, true ) );
    EXPECT_EQ( 0,  pns::countAsteroid( topology()->asteroid.content, true ) );

    // состо€ние астероида
    {
        // событи€
        const auto l = listenerAsteroid();
        //std::cout << *l << std::endl;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION ) ) << *l;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION, pns::GE_STAR ) ) << *l;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_DESTROY ) ) << *l;
        // астероид перемещаетс€
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_CHANGE_COORD ) ) << *l;
        // у астероида мен€етс€ скорость из-за действи€ гравитации звезды
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_CHANGE_VELOCITY ) ) << *l;
        // причина изм. скорости - ускорение из-за воздействи€ силы
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_IMPACT_ACCELERATION ) ) << *l;
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_IMPACT_FORCE ) ) << *l;
    }

    // состо€ние звезды
    {
        EXPECT_GT( tsc()[ 0 ].today.mass,  massStar );

        // событи€
        const auto l = listenerStar();
        //std::cout << *l << std::endl;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION ) ) << *l;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION, pns::GE_ASTEROID ) ) << *l;
        // масса звезды уменьшаетс€ каждый пульс из-за излучени€
        // и увеличиваетс€ из-за падени€ астероида
        EXPECT_EQ( PL + 1,  l->countByUIDEvent( pns::E_CHANGE_MASS ) ) << *l;
        // звезда прит€гивает тела гравитацией
        EXPECT_EQ( PL,  l->countByUIDEvent( pns::E_GRAVITY ) ) << *l;
        // звезда каждый пульс излучает энергию
        EXPECT_EQ( PL,  l->countByUIDEvent( pns::E_RADIATION ) ) << *l;
    }

#endif
}


} // namespace
