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
* Класс-родитель для тестов звёздной системы.
*/
class StarSystemTest : public ::testing::Test {
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
        mPortulan( new pns::Portulan() )
    {
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




private:
    std::shared_ptr< pes::Engine >    mEngine;
    std::shared_ptr< pns::Portulan >  mPortulan;
};



} // namespace
