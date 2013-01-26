#pragma once

#include "StarSystemTest.h"


namespace {


/**
* ������������ ����������� ��������� ��� ��������� � ������� �������.
*/
class AsteroidUniqueBehaviuorSST : public StarSystemTest {
protected:
    inline AsteroidUniqueBehaviuorSST(
    ) {
    }




    virtual inline ~AsteroidUniqueBehaviuorSST() {
    }




    /**
    * @virtual StarSystemTest
    */
    virtual inline void SetUp() {
        static const pns::aboutStarSystem_t aboutStarSystem = {
            // size
            // ~ ������ ~ 10000 �.�. ~ 1.5e12 ��
            { 1.5e15, 1.5e15, 1.5e15 }
        };
        topology()->aboutStarSystem = aboutStarSystem;


        // # ������ - ����� ���������.
        auto& tsc = topology()->star.content;
        size_t countStar = 0;

        // ������
        // @source ������ > http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
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

        // ��������� ������ ���� ��������
        static const pns::aboutStar_t STAR_END_LIST = {};
        tsc[ countStar ] = STAR_END_LIST;

        
        // ������� ����������
        engine()->addListenerAsteroid( listenerAsteroid(), nullptr, nullptr );
        engine()->addListenerStar( listenerStar(), nullptr, nullptr );
    }




    inline pns::real_t luminosityStar() const {
        const auto& tsc = topology()->star.content;
        return typelib::compute::physics::luminosity(
            tsc[ 0 ].today.radius,  tsc[ 0 ].today.surfaceTemperature
        );
    }
};







TEST_F( AsteroidUniqueBehaviuorSST,  Asteroid1GrowUpEatSunLight ) {
#if 1

    // ����������
#if 1

    // ��������
    // @source http://ru.wikipedia.org/wiki/%D0%90%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    auto& tac = topology()->asteroid.content;
    size_t countAsteroid = 0;

    // ��� ���������������� ����������
    static const pns::real_t asteroidOrbit = 1.49598261e11 / 50;

#if 1
    {
        const pns::uid_t uid = 1;
        // # ������ ��������� ������ �����, ����� ����� ������ �����������
        //   ��� ����������� �� �������� ��-�� ��������� �����.
        //   ��. ���������� ���� OpenCL direct() ��� ������.
        const pns::real_t rx = 30e3;
        const pns::real_t ry = 50e3;
        const pns::real_t rz = 40e3;
        pns::real3_t size = {};
        size.s[ 0 ] = rx;
        size.s[ 1 ] = ry;
        size.s[ 2 ] = rz;

        const pns::real_t density = 5000.0;
        const pns::real_t mass =
            typelib::compute::geometry::ellipsoid::volume( rx, ry, rz ) *
            density;

        const pns::real_t albedo = 0.6;

        const auto tsc = topology()->star.content;
        const pns::real_t radiusStar = tsc[ 0 ].today.radius;
        const pns::real_t surfaceTemperatureStar =
            tsc[ 0 ].today.surfaceTemperature;

        const pns::real_t d = asteroidOrbit;
        const pns::real_t temperature =
            typelib::compute::physics::effectiveTemperature(
                luminosityStar(), d, albedo
            );

        const pns::real3_t coord = { d, 0, 0 };

        pns::real3_t rotation = {};

        const auto& star = topology()->star;
        const auto absOrbitalSpeed =
            typelib::compute::physics::orbitalSpeed(
                mass,
                star.content[ 0 ].today.mass,
                d
            );
        const auto orbitalSpeed =
            typelib::VectorT< pns::real_t >( 0, absOrbitalSpeed, 0 );
        pns::real3_t velocity = {};
        velocity.s[ 0 ] = orbitalSpeed.x;
        velocity.s[ 1 ] = orbitalSpeed.y;
        velocity.s[ 2 ] = orbitalSpeed.z;

        const pns::characteristicAsteroid_t today = {
            // live
            true,
            // mass
            mass,
            // size
            size,
            // coord
            coord,
            // rotation
            rotation,
            // velocity
            velocity,
            // density
            // # ~ ������� 2330 �� / �3
            density,
            // temperature
            temperature,
            // albedo
            albedo,
            // meltingPoint
            // # ~ ������� 1680 �
            1600,
            // boilingPoint
            // # ~ ������� 2620 �
            2600,
            // heatCapacity
            // # ~ ������� 800 �� / (�� * �)
            800.0,
            // enthalpyFusion
            // # ~ ������� 50.6 ��� / ����
            50000 / 28.0855,
            // enthalpyVaporization
            // # ~ ������� 383 ��� / ����
            380000 / 28.0855
        };
        pns::aboutAsteroid_t asteroid = {
            // uid
            uid,
            // today
            today,
            // future
            {},
            // emitterEvent
            {},
            // memoryModel
            {}
        };

        // ������ ���� ���������� �������� ����������� ���������:
        // �� ����� �����, ������� ������ ������
        {
            const char model[] = "grow-up";
            EXPECT_FALSE( needPulseModelAsteroid( &asteroid, model, 0 ) );
            pns::learnModelAsteroid( &asteroid, model );
            // �������, ��� ����� ������ �������������� ��� ���������
            // 0 - ������ �����
            pns::frequenceModelAsteroid( &asteroid, model, 0 );
            EXPECT_TRUE( needPulseModelAsteroid( &asteroid, model, 0 ) );
        }

        tac[ countAsteroid ] = asteroid;
        ++countAsteroid;
    }
#endif

    // ��������� ������ ���������� ��������
    static const pns::aboutAsteroid_t ASTEROID_END_LIST = {};
    tac[ countAsteroid ] = ASTEROID_END_LIST;


    // ���������
    engine()->incarnate( portulan() );


    // ��������� ������������ ���������?
    testAsteroid( false );


    // ������� ���������
    pnios::VolumeVTKVisual::option_t  o;
    o[ "extent" ] = engine()->extent();
    o[ "auto-scale-camera" ] = false;
    o[ "without-clear" ] = false;
    o[ "size-window" ] = 1000;

    pnios::VolumeVTKVisual  visual( o );
    visual << *portulan();

#endif  // /����������
    

    // ������ ������ ���� (��. SetUp() � ����)
    const auto massStar = tsc()[ 0 ].today.mass;
    const auto allMassAsteroid = tsc()[ 0 ].today.mass;


    // ��������� ���
    // ����� ����� ���-�� �����, ����� �������� ����� ������ �� ������
    static const int needStep = 10 * 10000;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine().get() );

    // ������� ������� ��������� ������
    const auto PL = engine()->live().pulselive();


    // ��������� ���������

    // ��� �� �������������� ��������� �������� � �������� �����
    testAsteroid( false );
    // @todo testStar( false );

    EXPECT_EQ( 1,  pns::countStar( topology()->star.content, true ) );
    EXPECT_EQ( 0,  pns::countAsteroid( topology()->asteroid.content, true ) );

    // ��������� ���������
    {
        // �������
        const auto l = listenerAsteroid();
        //std::cout << *l << std::endl;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION ) ) << *l;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION, pns::GE_STAR ) ) << *l;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_DESTROY ) ) << *l;
        // �������� ������������
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_CHANGE_COORD ) ) << *l;
        // � ��������� �������� �������� ��-�� �������� ���������� ������
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_CHANGE_VELOCITY ) ) << *l;
        // ������� ���. �������� - ��������� ��-�� ����������� ����
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_IMPACT_ACCELERATION ) ) << *l;
        EXPECT_LE( 1u,  l->countByUIDEvent( pns::E_IMPACT_FORCE ) ) << *l;
    }

    // ��������� ������
    {
        EXPECT_GT( tsc()[ 0 ].today.mass,  massStar );

        // �������
        const auto l = listenerStar();
        //std::cout << *l << std::endl;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION ) ) << *l;
        EXPECT_EQ( 1u,  l->countByUIDEvent( pns::E_COLLISION, pns::GE_ASTEROID ) ) << *l;
        // ����� ������ ����������� ������ ����� ��-�� ���������
        // � ������������� ��-�� ������� ���������
        EXPECT_EQ( PL + 1,  l->countByUIDEvent( pns::E_CHANGE_MASS ) ) << *l;
        // ������ ����������� ���� �����������
        EXPECT_EQ( PL,  l->countByUIDEvent( pns::E_GRAVITY ) ) << *l;
        // ������ ������ ����� �������� �������
        EXPECT_EQ( PL,  l->countByUIDEvent( pns::E_RADIATION ) ) << *l;
    }

#endif
}


} // namespace
