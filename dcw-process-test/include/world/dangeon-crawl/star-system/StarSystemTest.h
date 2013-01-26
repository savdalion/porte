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
* Влияет на точность рассчётов (больше - ниже).
* Может быть задействован вместе с 'PULSE'.
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
* Слушатель для подсчёта кол-ва событий у элемента звёздной системы.
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
    * @return Количество событий с заданным 'uid' события.
    */
    inline size_t countByUIDEvent(
        enum pns::EVENT uid
    ) const {
        const auto ftr = storeCountByUIDEvent.find( uid );
        return (ftr == storeCountByUIDEvent.cend()) ? 0 : ftr->second;
    }




    /**
    * @return Количество событий с заданным 'uid' события, в которых первый
    *         участник - с заданным 'uu' в качестве UID элемента.
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
    * @return Количество событий с заданным 'uid' события, в которых второй
    *         участник - из группы элементов 'ge'.
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
    * @return Количество событий с заданным 'uid' события, в которых второй
    *         участник - из группы элементов 'ge' с заданным 'uu' в
    *         качестве UID элемента.
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
        // соберём события
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
    * Перечисление всех отловленных событий.
    */
    typedef std::pair< pns::pointerElement_t, pns::eventTwo_t >  all_t;
    std::vector< all_t >  storeAll;

    /**
    * Количество событий по типам события.
    */
    std::map< enum pns::EVENT, size_t >  storeCountByUIDEvent;

    /**
    * Количество событий по группам элементов.
    */
    std::map< enum pns::GROUP_ELEMENT, size_t >  storeCountByGroupElement;

    /**
    * Количество событий по UID элементов.
    */
    std::map< pns::uid_t, size_t >  storeCountByUIDElement;
};




/**
* Класс-родитель для тестов звёздной системы.
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
        // Инициализируем движок звёздной системы
        // # Инициализация происходит частичная, чтобы в процессе формирования
        //   портулана звёзной системы можно было привязывать к элементам
        //   другие движки, подписывая их на события друг друга. См. wrapPlanet().
        // # Движок оборачиваем в shared_ptr, т.к. он будет отдаваться как
        //   слушатель событий другим движкам.
        mEngine( new pes::Engine( TIMESTEP ) ),
        mPortulan( new pns::Portulan() ),
        // создадим здесь слушателей событий, иначе они будут тиху удалены
        // при попытке добавить их в тестах (из-за shared_ptr)
        mListenerAsteroid( new CounterEventListenerAsteroid() ),
        mListenerStar( new CounterEventListenerStar() )
    {
        // # Нужные тестам слушатели подключаются к движкам в самих тестах.
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
    * Проверяет элементы звёздной системы на корректность содержания.
    *
    * @param onlyLive Просматривать только живые - present*() - элементы.
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

    // хватило разрядности для хранения характеристик
    for (size_t i = 0; i < pns::ASTEROID_COUNT; ++i) {
        const pns::aboutAsteroid_t& element = tac()[ i ];

        //std::cout << i << " " << element.uid << std::endl;

        if ( onlyLive && pns::absentAsteroid( &element ) ) {
            // # Всегда просматриваем весь список, на случай если он
            //   оказался неоптимизирован - см. pns::optimize*().
            continue;
        }

        // общие характеристики

        EXPECT_TRUE( testReal( element.today.coord.s[ 0 ] ) ) <<
            "Координата X не корректна";
        EXPECT_TRUE( testReal( element.today.coord.s[ 1 ] ) ) <<
            "Координата Y не корректна";
        EXPECT_TRUE( testReal( element.today.coord.s[ 2 ] ) ) <<
            "Координата Z не корректна";

        EXPECT_TRUE( testReal( element.today.mass ) ) <<
            "Масса не корректна";

        EXPECT_TRUE( testReal( element.today.density ) ) <<
            "Плотность не корректна";

        EXPECT_TRUE( testReal( element.today.velocity.s[ 0 ] ) ) <<
            "Скорость X не корректна";
        EXPECT_TRUE( testReal( element.today.velocity.s[ 1 ] ) ) <<
            "Скорость Y не корректна";
        EXPECT_TRUE( testReal( element.today.velocity.s[ 2 ] ) ) <<
            "Скорость Z не корректна";

        
        // память событий
        for (int w = pns::EMITTER_EVENT_COUNT - 1; w >= 0; --w) {
            const pns::eventTwo_t& e = element.emitterEvent.content[ w ];
            EXPECT_TRUE( (e.uid >= pns::E_NONE) && (e.uid < pns::E_last) ) <<
                "Событие не определено";
            for (size_t q = 0; q < pns::FEATURE_EVENT_COUNT; ++q) {
                EXPECT_TRUE( testReal( e.fReal[ q ] ) ) <<
                    "Значение " << q << " в ячейке эммитера " << w <<
                    " для события " << e.uid << " не корректно." <<
                    " Астероид " << i << " (" << element.uid << ").";
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
