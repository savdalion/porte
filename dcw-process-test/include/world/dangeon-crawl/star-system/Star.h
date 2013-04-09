#pragma once

#include "StarSystemTest.h"


namespace {


/**
* ������������ ������ � ������� �������.
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
            // ~ ������ ~ 10000 �.�. ~ 1.5e12 ��
            { 1.5e15, 1.5e15, 1.5e15 }
        };
        topology()->aboutStarSystem = aboutStarSystem;


        // ������� ����������
        engine()->addListenerAsteroid( listenerAsteroid(), nullptr, nullptr );
        engine()->addListenerStar( listenerStar(), nullptr, nullptr );
    }

};







TEST_F( StarSST,  SingleStar ) {
#if 1

    // ����������
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
