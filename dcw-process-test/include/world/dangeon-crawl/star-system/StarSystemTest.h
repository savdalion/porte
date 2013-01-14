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
* �����-�������� ��� ������ ������� �������.
*/
class StarSystemTest : public ::testing::Test {
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
