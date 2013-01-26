#pragma once

#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>


// ���������� � "porte" ��� ������������� �������� C++ ��� OpenCL.
#undef PORTULAN_AS_OPEN_CL_STRUCT

// (!) ������� ����������� define, �������� � ������ cl.h � wingdi.h, �.�.
// ��� ��������� � ������ ����� ��������.
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
* # ����� - ���������, ���������� ������ ��� ���������. ����������� � ��������.
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
* ������ �� �������� ��������� (������ - ����).
* ����� ���� ������������ ������ � 'PULSE'.
*/
static const double TIMESTEP = MINUTE;


/*
* # ��������� �������. ������ ������� �������, � ������ - 'timestep'.
* 'PULSE' ������ �� ���-�� ��������� (������ ����� - ������ ������).
* ����� ���� ������������� ������ � 'timestep'.
* ������ ������ ������� 'PULSE' ������ � ����� 'timestep' � ������
* ����� - ���������� ��������. ��. �������, �������� ����� ������������
* ������ = timestep * PULSE.
* @example timestep = HOUR,  PULSE = 365 * 24 - ����� ����� ����������
*          ����� �����������, �.�. � ������ ��������� ~ 365 ����.
* @example timestep = MINUTE,  PULSE = 60 - �������������� ������ �����
*          � ��������� �� 1 ������, ������������ - ������ ��� ����� �����.
*/
static const int PULSE = 60;




/**
* ��������� ��� �������� ���-�� ������� � �������� ������� �������.
*/
template< class L >
class CounterEventListener :
    public L
{
public:
    inline CounterEventListener() {
    }


    virtual inline ~CounterEventListener() {
    }




    /**
    * @return ���������� ������� � �������� 'uid' �������.
    */
    inline size_t countByUIDEvent(
        enum pns::EVENT uid
    ) const {
        const auto ftr = storeCountByUIDEvent.find( uid );
        return (ftr == storeCountByUIDEvent.cend()) ? 0 : ftr->second;
    }




    /**
    * @return ���������� ������� � �������� 'uid' �������, � ������� ������
    *         �������� - � �������� 'uu' � �������� UID ��������.
    */
    inline size_t countByUIDEvent(
        enum pns::EVENT uid,
        pns::uid_t uu
    ) const {
        size_t n = 0;
        for (auto itr = storeAll.cbegin(); itr != storeAll.cend(); ++itr) {
            const pns::pointerElement_t& piA = itr->first;
            const pns::eventTwo_t& e = itr->second;
            if ( (piA.uu == uu) && (e.uid == uid) ) {
                ++n;
            }
        }
        return n;
    }




    /**
    * @return ���������� ������� � �������� 'uid' �������, � ������� ������
    *         �������� - �� ������ ��������� 'ge'.
    */
    inline size_t countByUIDEvent(
        enum pns::EVENT uid,
        enum pns::GROUP_ELEMENT ge
    ) const {
        size_t n = 0;
        for (auto itr = storeAll.cbegin(); itr != storeAll.cend(); ++itr) {
            const pns::eventTwo_t& e = itr->second;
            const pns::pointerElement_t& piB = e.pi;
            if ( (e.uid == uid) && (piB.ge == ge) ) {
                ++n;
            }
        }
        return n;
    }




    /**
    * @return ���������� ������� � �������� 'uid' �������, � ������� ������
    *         �������� - �� ������ ��������� 'ge' � �������� 'uu' �
    *         �������� UID ��������.
    */
    inline size_t countByUIDEvent(
        enum pns::EVENT uid,
        enum pns::GROUP_ELEMENT ge,
        pns::uid_t uu
    ) const {
        size_t n = 0;
        for (auto itr = storeAll.cbegin(); itr != storeAll.cend(); ++itr) {
            const pns::eventTwo_t& e = itr->second;
            const pns::pointerElement_t& piB = e.pi;
            if ( (e.uid == uid) && (piB.ge == ge) && (piB.uu == uu) ) {
                ++n;
            }
        }
        return n;
    }




private:
    virtual inline void beforeEvent(
        const pns::pointerElement_t& piA,
        const pns::eventTwo_t& e
    ) {
        // ������ �������
        {
            storeAll.push_back( std::make_pair( piA, e ) );
        }
        {
            auto r = storeCountByUIDEvent.insert( std::make_pair( e.uid, 0 ) );
            ++r.first->second;
        }
        {
            auto r = storeCountByGroupElement.insert( std::make_pair( piA.ge, 0 ) );
            ++r.first->second;
        }
        {
            auto r = storeCountByUIDElement.insert( std::make_pair( piA.uu, 0 ) );
            ++r.first->second;
        }
    }




public:
    /**
    * ������������ ���� ����������� �������.
    */
    typedef std::pair< pns::pointerElement_t, pns::eventTwo_t >  all_t;
    std::vector< all_t >  storeAll;

    /**
    * ���������� ������� �� ����� �������.
    */
    std::map< enum pns::EVENT, size_t >  storeCountByUIDEvent;

    /**
    * ���������� ������� �� ������� ���������.
    */
    std::map< enum pns::GROUP_ELEMENT, size_t >  storeCountByGroupElement;

    /**
    * ���������� ������� �� UID ���������.
    */
    std::map< pns::uid_t, size_t >  storeCountByUIDElement;
};




/**
* �����-�������� ��� ������ ������� �������.
*/
class StarSystemTest : public ::testing::Test {
public:
    typedef CounterEventListener< pes::ListenerAsteroid >
        CounterEventListenerAsteroid;
    typedef CounterEventListener< pes::ListenerStar >
        CounterEventListenerStar;

protected:
    inline StarSystemTest(
    ) :
        // �������������� ������ ������� �������
        // # ������������� ���������� ���������, ����� � �������� ������������
        //   ��������� ������ ������� ����� ���� ����������� � ���������
        //   ������ ������, ���������� �� �� ������� ���� �����. ��. wrapPlanet().
        // # ������ ����������� � shared_ptr, �.�. �� ����� ���������� ���
        //   ��������� ������� ������ �������.
        mEngine( new pes::Engine( TIMESTEP ) ),
        mPortulan( new pns::Portulan() ),
        // �������� ����� ���������� �������, ����� ��� ����� ���� �������
        // ��� ������� �������� �� � ������ (��-�� shared_ptr)
        mListenerAsteroid( new CounterEventListenerAsteroid() ),
        mListenerStar( new CounterEventListenerStar() )
    {
        // # ������ ������ ��������� ������������ � ������� � ����� ������.
        // @example engine()->addListenerStar( listenerStar() );
    }




    virtual inline ~StarSystemTest() {
    }




    virtual inline void SetUp() = 0;




    virtual void TearDown() {
    };




    inline std::shared_ptr< pes::Engine >  engine() {
        return mEngine;
    }




    inline std::shared_ptr< pns::Portulan >  portulan() {
        return mPortulan;
    }




    inline pns::topology_t* topology() {
        return &mPortulan->topology().topology();
    }




    inline pns::topology_t const* topology() const {
        return &mPortulan->topology().topology();
    }




    inline pns::asteroidContent_t asteroidContent() const {
        return mPortulan->topology().topology().asteroid.content;
    }

    inline pns::asteroidContent_t asteroidContent() {
        return mPortulan->topology().topology().asteroid.content;
    }

    inline pns::asteroidContent_t tac() const {
        return asteroidContent();
    }

    inline pns::asteroidContent_t tac() {
        return asteroidContent();
    }




    inline pns::planetContent_t planetContent() const {
        return mPortulan->topology().topology().planet.content;
    }

    inline pns::planetContent_t planetContent() {
        return mPortulan->topology().topology().planet.content;
    }

    inline pns::planetContent_t tpc() const {
        return planetContent();
    }

    inline pns::planetContent_t tpc() {
        return planetContent();
    }




    inline pns::starContent_t starContent() const {
        return mPortulan->topology().topology().star.content;
    }

    inline pns::starContent_t starContent() {
        return mPortulan->topology().topology().star.content;
    }

    inline pns::starContent_t tsc() const {
        return starContent();
    }

    inline pns::starContent_t tsc() {
        return starContent();
    }




    inline std::shared_ptr< CounterEventListenerAsteroid >  listenerAsteroid() {
        return std::static_pointer_cast< CounterEventListenerAsteroid >(
            mListenerAsteroid
        );
    }




    inline std::shared_ptr< CounterEventListenerStar >  listenerStar() {
        return std::static_pointer_cast< CounterEventListenerStar >(
            mListenerStar
        );
    }




protected:
    /**
    * ��������� �������� ������� ������� �� ������������ ����������.
    *
    * @param onlyLive ������������� ������ ����� - present*() - ��������.
    */
    void testAsteroid( bool onlyLive ) const;




    static inline bool testReal( pns::real_t v ) {
        // @source http://johndcook.com/IEEE_exceptions_in_cpp.html
        return ( (v <= std::numeric_limits< pns::real_t >().max())
            && (v >= -std::numeric_limits< pns::real_t >().max())
        );
    }




    static inline bool testReal4( pns::real4_t v ) {
        return
            testReal( v.s[ 0 ] )
         && testReal( v.s[ 1 ] )
         && testReal( v.s[ 2 ] )
         && testReal( v.s[ 3 ] );
    }




private:
    std::shared_ptr< pes::Engine >    mEngine;
    std::shared_ptr< pns::Portulan >  mPortulan;

    std::shared_ptr< pes::ListenerAsteroid >  mListenerAsteroid;
    std::shared_ptr< pes::ListenerStar >      mListenerStar;
};








inline void StarSystemTest::testAsteroid( bool onlyLive ) const {

    // ������� ����������� ��� �������� �������������
    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        const pns::aboutAsteroid_t& element = tac()[ i ];

        //std::cout << i << " " << element.uid << std::endl;

        if ( onlyLive && pns::absentAsteroid( &element ) ) {
            // # ������ ������������� ���� ������, �� ������ ���� ��
            //   �������� ��������������� - ��. pns::optimize*().
            continue;
        }

        // ����� ��������������

        EXPECT_TRUE( testReal( element.today.coord.s[ 0 ] ) ) <<
            "���������� X �� ���������";
        EXPECT_TRUE( testReal( element.today.coord.s[ 1 ] ) ) <<
            "���������� Y �� ���������";
        EXPECT_TRUE( testReal( element.today.coord.s[ 2 ] ) ) <<
            "���������� Z �� ���������";

        EXPECT_TRUE( testReal( element.today.mass ) ) <<
            "����� �� ���������";

        EXPECT_TRUE( testReal( element.today.density ) ) <<
            "��������� �� ���������";

        EXPECT_TRUE( testReal( element.today.velocity.s[ 0 ] ) ) <<
            "�������� X �� ���������";
        EXPECT_TRUE( testReal( element.today.velocity.s[ 1 ] ) ) <<
            "�������� Y �� ���������";
        EXPECT_TRUE( testReal( element.today.velocity.s[ 2 ] ) ) <<
            "�������� Z �� ���������";

        
        // ������ �������
        for (int w = pns::EMITTER_EVENT_COUNT - 1; w >= 0; --w) {
            const pns::eventTwo_t& e = element.emitterEvent.content[ w ];
            EXPECT_TRUE( (e.uid >= pns::E_NONE) && (e.uid < pns::E_last) ) <<
                "������� �� ����������";
            for (size_t q = 0; q < pns::FEATURE_EVENT_COUNT; ++q) {
                EXPECT_TRUE( testReal( e.fReal[ q ] ) ) <<
                    "�������� " << q << " � ������ �������� " << w <<
                    " ��� ������� " << e.uid << " �� ���������." <<
                    " �������� " << i << " (" << element.uid << ").";
            }
        }

    } // for (size_t i = 0 ...

}



} // namespace









namespace std {


template< class L >
inline std::ostream& operator<<(
    std::ostream&  out,
    const CounterEventListener< L >&  cel
) {
    out << "Count all events:  ";
    out << cel.storeAll.size();
    out << std::endl;

    out << "Count events by UIDs of events:  ";
    typelib::printPair( out, cel.storeCountByUIDEvent );
    out << std::endl;

    out << "Count events by groups of elements:  ";
    typelib::printPair( out, cel.storeCountByGroupElement );
    out << std::endl;

    out << "Count events by UIDs of elements:  ";
    typelib::printPair( out, cel.storeCountByUIDElement );
    out << std::endl;

    return out;
}


} // std
