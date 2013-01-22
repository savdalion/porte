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




private:
    std::shared_ptr< pes::Engine >    mEngine;
    std::shared_ptr< pns::Portulan >  mPortulan;

    std::shared_ptr< pes::ListenerAsteroid >  mListenerAsteroid;
    std::shared_ptr< pes::ListenerStar >      mListenerStar;
};



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
