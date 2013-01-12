#include "../include/StdAfx.h"
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


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;
namespace pnios = portulan::io::world::dungeoncrawl::starsystem::l0;

namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;
namespace pniop = portulan::io::world::dungeoncrawl::planet::l0;


// # ������ ��� ��������� � ������ � ���������, ����� �� ���������� ������
//   ��������� �� ���� �� ��� �� ��� ��������� (�������, std::shared_ptr).
typedef struct {
    std::set< std::shared_ptr< pnp::Portulan > >  planet;
    std::set< std::shared_ptr< pns::Portulan > >  starSystem;
} heapPortulan_t;

typedef struct {
    std::set< std::shared_ptr< pep::Engine > >  planet;
    std::set< std::shared_ptr< pes::Engine > >  starSystem;
} heapEngine_t;

typedef struct {
    heapPortulan_t  portulan;
    heapEngine_t    engine;
} heap_t;

static heap_t heap;



static void wrapPlanet( heap_t*,  std::shared_ptr< pes::Engine >,  const pns::aboutPlanet_t& );


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
* ���������� ������������ ��� ������� 'Porte'.
* ������ �������� ��������� �������.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace tc = typelib::constant::physics;


    // ��������� ���������� ������ ���-�����
    axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_rarely );
    //axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_often );

    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> BEGIN" << std::endl;
    #ifdef _MSC_VER
        #ifdef _DEBUG
            EZLOGGERVLSTREAM( axter::log_always ) << "Debug." << std::endl;
        #else
            EZLOGGERVLSTREAM( axter::log_always ) << "Release." << std::endl;
        #endif
    #endif

            
    setlocale( LC_ALL, "Russian" );
    // ��� ����������� '.' ������ ','
    setlocale( LC_NUMERIC, "C" );




// ������� �������
// @todo fine ��������� ������� �� ��. ������� ������ >
//       http://ru.wikipedia.org/wiki/%D0%A1%D0%BF%D0%B8%D1%81%D0%BE%D0%BA_%D1%8D%D0%BA%D0%B7%D0%BE%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82_%D0%B2_%D0%BE%D0%B1%D0%B8%D1%82%D0%B0%D0%B5%D0%BC%D0%BE%D0%B9_%D0%B7%D0%BE%D0%BD%D0%B5
#if 1
    // �������������� ������ ������� �������
    // # ������������� ���������� ���������, ����� � �������� ������������
    //   ��������� ������ ������� ����� ���� ����������� � ���������
    //   ������ ������, ���������� �� �� ������� ���� �����. ��. wrapPlanet().
    // 'timestep' ������ �� �������� ��������� (������ - ����).
    // ����� ���� ������������� ������ � 'PULSE'.
    static const double timestep = MINUTE * 5;
    // # ������ ����������� � shared_ptr, �.�. �� ����� ���������� ���
    //   ��������� ������� ������ �������.
    std::shared_ptr< pes::Engine >  engine( new pes::Engine( timestep ) );
    heap.engine.starSystem.insert( engine );


    typedef pns::Portulan  starSystem_t;
    std::shared_ptr< starSystem_t >  starSystem( new starSystem_t() );
    heap.portulan.starSystem.insert( starSystem );

    pns::topology_t& topology = starSystem->topology().topology();

    static const pns::aboutStarSystem_t aboutStarSystem = {
        // size
        // ~ ������ ~ 10000 �.�. ~ 1.5e12 ��
        { 1.5e15, 1.5e15, 1.5e15 }
    };

    // ��� ���������� ���� ����������
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    //static const pns::real_t asteroidOrbit = 1.49598261e11 * 2.8;
    static const pns::real_t asteroidOrbit = 1.49598261e11;

    topology.aboutStarSystem = aboutStarSystem;



    // �����
    // # ������ - ����� ���������.
    auto& tsc = topology.star.content;
    size_t countStar = 0;

#if 1
    // ������ I
    // �����
    // @source ������ > http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
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
            // kernelTemperature,
            13.5e6,
            // surfaceTemperature,
            1.5e6,
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

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_STAR, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countStar;
    }
#endif

    // ������ II
    // �����
    // @source ������ > http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
#if 0
    {
        static const pns::uid_t uid = 2;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            1.9891e30,  1.9891e30 + 1.9891e30,  (6.9551e8 * 70.0) * 2.0
        ) / 1.9;
        static const pns::aboutStar_t star = {
            // uid
            uid,
            // live
            true,
            // mass
            1.9891e30,
            // radius
            6.9551e8,
            // kernelTemperature,
            13.5e6,
            // surfaceTemperature,
            1.5e6,
            // coord
            { -6.9551e8 * 70.0, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0, -orbitalSpeed, 0 }
        };
        tsc[ countStar ] = star;

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_STAR, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countStar;
    }
#endif

    // ������ III
    // ������
    // @source ������ > http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
#if 0
    {
        static const pns::uid_t uid = 3;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            1.9891e30,  1.9891e30 + 1.9891e30,  (6.9551e8 * 70.0) * 2.0
        ) / 1.9;
        static const pns::aboutStar_t star = {
            // uid
            uid,
            // live
            true,
            // mass
            1.9891e30,
            // radius
            6.9551e8,
            // kernelTemperature,
            13.5e6,
            // surfaceTemperature,
            1.5e6,
            // coord
            { 6.9551e8 * 70.0, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0, orbitalSpeed, 0 }
        };
        tsc[ countStar ] = star;

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_STAR, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countStar;
    }
#endif

    // ��������� ������ ��������
    static const pns::aboutStar_t STAR_END_LIST = {};
    tsc[ countStar ] = STAR_END_LIST;

#endif // �����



    // �������
    auto& tpc = topology.planet.content;
    size_t countPlanet = 0;

    // #! ��� ���������� ����� ���� (������ I == ������), ��� ������ �������
    //    ������� �������������� ��������.

#if 0
    // ��������
    // @source http://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%80%D0%BA%D1%83%D1%80%D0%B8%D0%B9_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 0
    {
        static const pns::uid_t uid = 1;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            3.33022e23,  1.9891e30 + 1.9891e30,  0.4600121e11
        );
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // live
            true,
            // mass
            3.33022e23,
            // radius
            2.4397e6,
            // coord
            { 0.4600121e11, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            //{ 0, 47870, 0 }
            { 0, orbitalSpeed, 0 }
        };
        tpc[ countPlanet ] = planet;
        ++countPlanet;
    }
#endif

    // ������
    // @source http://ru.wikipedia.org/wiki/%D0%92%D0%B5%D0%BD%D0%B5%D1%80%D0%B0_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 1
    {
        static const pns::uid_t uid = 2;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            4.8685e24,  1.9891e30 + 1.9891e30,  1.07476259e11
        );
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // live
            true,
            // mass
            4.8685e24,
            // radius
            6.0518e6,
            // coord
            { 0, 1.07476259e11, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            //{ 0, 35020, 0 }
            { orbitalSpeed, 0, 0 }
        };
        tpc[ countPlanet ] = planet;

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_PLANET, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countPlanet;
    }
#endif

    // �����
    // @source http://ru.wikipedia.org/wiki/%D0%97%D0%B5%D0%BC%D0%BB%D1%8F
#if 1
    {
        static const pns::uid_t uid = 3;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            5.9736e24,  1.9891e30 + 1.9891e30,  1.49598261e11
        );
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // live
            true,
            // mass
            5.9736e24,
            // radius
            6.3568e6,
            // coord
            { 0, -1.49598261e11, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            //{ 0, 29783, 0 },
            { -orbitalSpeed, 0, 0 },
            // axilTilt
            23.44,
            // rotationPeriod
            0.99726968 * 24 * 60 * 60
        };
        tpc[ countPlanet ] = planet;

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_PLANET, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countPlanet;

        // ��� �� ����������� ������� � ���� ������
        wrapPlanet( &heap, engine, planet );
    }
#endif


    // ����
    // @source http://ru.wikipedia.org/wiki/%D0%9C%D0%B0%D1%80%D1%81_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 1
    {
        static const pns::uid_t uid = 4;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            0.64185e24,  1.9891e30 + 1.9891e30,  2.06655e11
        );
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // live
            true,
            // mass
            0.64185e24,
            // radius
            3.3895e6,
            // coord
            { 2.06655e11, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            //{ 0, 24130, 0 }
            { 0, orbitalSpeed, 0 }
        };
        tpc[ countPlanet ] = planet;

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_PLANET, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countPlanet;
    }
#endif


    // ���
#if 1
    {
        static const pns::uid_t uid = 11;
        static const auto orbitalSpeed = typelib::compute::physics::orbitalSpeed(
            1.1e24,  1.9891e30 + 1.9891e30,  4.4e11
        ) * 0.6;
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // live
            true,
            // mass
            1.1e24,
            // radius
            5.5e6,
            // coord
            { -4.4e11, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0, orbitalSpeed, 0 }
        };
        tpc[ countPlanet ] = planet;

        // ������ �� ���� ����������
        static const pns::uidElement_t uide = { pns::GE_PLANET, uid };
        starSystem->addOrderStatistics( uide, 50 );

        ++countPlanet;
    }
#endif

    // ��������� ������ ��������
    static const pns::aboutPlanet_t PLANET_END_LIST = {};
    tpc[ countPlanet ] = PLANET_END_LIST;

#endif // �������



    // ���������
    // @source http://ru.wikipedia.org/wiki/%D0%90%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    static const size_t N_ASTEROID = 1000;
    assert( (N_ASTEROID <= pns::ASTEROID_COUNT) &&
        "���������� ���������� ��������� ����������������� ��� ��� �����. ��. ASTEROID_COUNT." );
    size_t countAsteroid = 0;

#if 1
    auto& tac = topology.asteroid.content;
    {
        // �� ��������� ��������� ����. �����: ������� ������
        // ��� ������ ���������� (�������������) ����������
        static const size_t SEED = 12345;
        typelib::Random< size_t >  seed( 0, 1000000000, SEED );

        typelib::Random< pns::real_t >
            averageRadius( 100, 300000, seed.next() );
        typelib::Random< pns::real_t >
            proportionRadius( 0.5, 1.5, seed.next() );
        typelib::Random< pns::real_t >
            density( 2000, 4000, seed.next() );

        typelib::Random< pns::real_t >
            distance( asteroidOrbit * 0.9,  asteroidOrbit * 1.1,  seed.next() );
        typelib::Random< pns::real_t >
            angle( 0.0,  M_PI * 2.0,  seed.next() );

        static const pns::real_t velocityOrbit = 17500;
        typelib::Random< pns::real_t >
            velocity( velocityOrbit * 0.95,  velocityOrbit * 1.05,  seed.next() );

        const pns::real_t radiusStar = tsc[ 0 ].radius;
        const pns::real_t surfaceTemperatureStar = tsc[ 0 ].surfaceTemperature;
        const pns::real_t luminosityStar =
            typelib::compute::physics::luminosity(
                radiusStar, surfaceTemperatureStar
            );

        for (size_t i = 0; i < N_ASTEROID; ++i) {
            // # ��������� �� ���������.
            const pns::real_t ar = averageRadius.next();
            const pns::real_t rx = ar * proportionRadius.next();
            const pns::real_t ry = ar * proportionRadius.next();
            const pns::real_t rz = ar * proportionRadius.next();

            const pns::real_t volume = 4.0 / 3.0 * M_PI * rx * ry * rz;
            const pns::real_t rdensity = density.next();
            const pns::real_t mass = volume * rdensity;

            const pns::real_t d  = distance.next();
            const pns::real_t da = angle.next();
            const pns::real_t cx = d * std::sin( da );
            const pns::real_t cy = d * std::cos( da );
            const pns::real_t cz = 0;

            const pns::real_t albedo = 0.6;

            const pns::real_t temperature =
                typelib::compute::physics::effectiveTemperature(
                    luminosityStar, d, albedo
                );

            // �������� - ��������������� ���� �� ������ ������� �������
            const pns::real_t _velocity = velocity.next();
            const pns::real_t vx = _velocity * std::sin( da - M_PI_2 ) * 0.0;
            const pns::real_t vy = _velocity * std::cos( da - M_PI_2 ) * 0.0;
            const pns::real_t vz = 0.0;

            const pns::aboutAsteroid_t asteroid = {
                // uid
                i + 1,
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
                { vx, vy, vz },
                // density
                // # ~ ������� 2330 �� / �3
                rdensity,
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
                380000 / 28.0855,
                // # memoryEvent
                { 0, {} }
            };
            tac[ countAsteroid ] = asteroid;
            ++countAsteroid;

        } // for (size_t i = 0; i < N; ++i)

        // ��������� ������ ��������
        static const pns::aboutAsteroid_t ASTEROID_END_LIST = {};
        tac[ countAsteroid ] = ASTEROID_END_LIST;
    }
#endif // ���������



    // @test
    std::cout << "asteroid" <<
        "  " << countAsteroid << " / " << pns::ASTEROID_COUNT << "u" <<
        "  ~ " << sizeof( pns::asteroid_t ) / 1024 << "��" <<
        std::endl << "planet" <<
        "  " << countPlanet << " / " << pns::PLANET_COUNT << "u" <<
        "  ~ " << sizeof( pns::planet_t ) / 1024 << "��" <<
        std::endl << "star" <<
        "  " << countStar << " / " << pns::STAR_COUNT << "u" <<
        "  ~ " << sizeof( pns::star_t ) / 1024 << "��" <<
        std::endl <<
    std::endl;



    // ��������� ������� �������
    engine->incarnate( starSystem );
    // #! starSystem = nullptr

#endif // ������� �������



    // ������� ��������� ��� ������� �������
    pnios::VolumeVTKVisual::option_t  o;
    o[ "extent" ] = engine->extent();
    o[ "asteroid-size-point" ] = 2;
    o[ "planet-size-point" ] = 5;
    o[ "star-size-point" ] = 10;
    o[ "auto-scale-camera" ] = false;
    o[ "without-clear" ] = false;
    o[ "size-window" ] = 1000;

    pnios::VolumeVTKVisual  visual( o );
    visual << *engine->portulan();
    

    // # ��������� �������. ������ ������� �������, � ������ - 'timestep'.
    // 'PULSE' ������ �� ���-�� ��������� (������ ����� - ������ ������).
    // ����� ���� ������������� ������ � 'timestep'.
    // ������ ������ ������� 'PULSE' ������ � ����� 'timestep' � ������
    // ����� - ���������� ��������. ��. �������, �������� ����� ������������
    // ������ = timestep * PULSE.
    // @example timestep = HOUR,  PULSE = 365 * 24 - ����� ����� ����������
    //          ����� �����������, �.�. � ������ ��������� ~ 365 ����.
    static const int PULSE = 60 * 12;

    // ��������� ���
    static const int needStep = 0;
    static const bool closeWindow = true;
    static const bool showPulse = true;
    visual.wait< 1, PULSE, needStep, closeWindow, showPulse >( engine.get() );




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()










/**
* ���������� � ������� ������� ������� ������.
*
* # ������ ������ � ������� � ������� ������� ������� � ��������� ��
*   ������ �������.
* # ��������� ������ ������� ��� ��������� ������� � ������� ������
*   ������� �������.
* # ��������� ������ ������� ������� ��� ��������� ������� �������. ��������,
*   ������ ������� ������� ������ ����� � ���������� / ������� �������.
*/
inline void wrapPlanet(
    heap_t*  heap,
    std::shared_ptr< pes::Engine >  ess,
    const pns::aboutPlanet_t&  pss
) {
    assert( heap );

    namespace tc = typelib::constant;


    // ��������� ��������� �������, �������� �� ������ 'planetStarSystem'
    /* - ���. ����� ������� ����� �������.
    static const std::string charUID = "earth";
    assert( (strcmp(planetStarSystem.charUID, charUID.c_str()) == 0)
        && "��������� �������� ������� � ������ UID" );
    */

    typedef pnp::Portulan  planet_t;
    std::shared_ptr< planet_t >  planet( new planet_t() );
    heap->portulan.planet.insert( planet );


    pnp::topology_t& topology = planet->topology().topology();

    // # ��� �� ������� - �������� � 'float'. ��. ����� ��������� �
    //   planet::Engine.
    typedef float  real_t;

    // ����� ���������� �� ������� �������
    // @source http://ru.wikipedia.org/wiki/%D0%97%D0%B5%D0%BC%D0%BD%D0%B0%D1%8F_%D0%BA%D0%BE%D1%80%D0%B0
    const real_t radiusCrust = pss.radius;
    const real_t radiusAtmosphere = radiusCrust + 200.0f * 1000.0f;
    const real_t halfSize = radiusAtmosphere;
    const real_t radiusMantle = radiusCrust - 200.0f * 1000.0f;
    const real_t radiusCore = radiusCrust * 0.4f;

    // @todo = f( ������ ������� �� ������� ������� )
    const real_t massAtmosphere = static_cast< real_t >( 0.000001 * pss.mass );
    const real_t massCrust =      static_cast< real_t >( 0.099999 * pss.mass );
    const real_t massMantle =     static_cast< real_t >( 0.6      * pss.mass );
    const real_t massCore =       static_cast< real_t >( 0.3      * pss.mass );
    const real_t resultMass = massAtmosphere + massCrust + massMantle + massCore;
    assert( typelib::equal( pss.mass, resultMass )
        && "����� ������� ������ ���� ����������� � ������ � ������� �������." );

    static const pnp::aboutPlanet_t aboutPlanet = {
#if 1
        // size
        halfSize * 2.0f,

        // radius
        { radiusAtmosphere / halfSize,  radiusCrust / halfSize,  radiusMantle / halfSize,  radiusCore / halfSize },

        // mass
        { massAtmosphere, massCrust, massMantle, massCore },

        // component
        {
            // space
            {
                { pnp::CC_NONE, 0.0f },
            },
            // atmosphere
            {
                { pnp::CC_AIR,        100.0f / 100.0f },
                { pnp::CC_NONE, 0.0f },
            },
            // crust
            {
                { pnp::CC_AIR,          3.0f / 100.0f },
                { pnp::CC_BARREN_SOIL, 14.0f / 100.0f },
                { pnp::CC_RICH_SOIL,    1.0f / 100.0f },
                { pnp::CC_SAND,        10.0f / 100.0f },
                { pnp::CC_ROCK,        20.0f / 100.0f },
                { pnp::CC_BOULDER,      4.0f / 100.0f },
                { pnp::CC_WATER,       47.0f / 100.0f },
                { pnp::CC_SPARSE,       0.5f / 100.0f },
                { pnp::CC_NONE, 0.0f },
            },
            // mantle
            {
                { pnp::CC_AIR,          1.0f / 100.0f },
                { pnp::CC_BARREN_SOIL, 12.0f / 100.0f },
                { pnp::CC_SAND,        20.0f / 100.0f },
                { pnp::CC_ROCK,        60.0f / 100.0f },
                { pnp::CC_WATER,        2.0f / 100.0f },
                { pnp::CC_SPARSE,       5.0f / 100.0f },
                { pnp::CC_NONE, 0.0f },
            },
            // core
            {
                { pnp::CC_AIR,          0.1f / 100.0f },
                { pnp::CC_ROCK,        90.0f / 100.0f },
                { pnp::CC_WATER,        0.9f / 100.0f },
                { pnp::CC_SPARSE,       9.0f / 100.0f },
                { pnp::CC_NONE, 0.0f },
            }
        },

        // temperature: center, surface
        {
            // space
            { tc::physics::CK + 1.0f,  tc::physics::CK + 1.0f },
            // atmosphere
            {   20.0f,  tc::physics::CK + 2.0f },
            // crust
            { 1000.0f,    20.0f },
            // mantle
            { 4000.0f,  1000.0f },
            // core
            { 6000.0f,  4000.0f }
        },

        // surfaceTemperature: pole, equator
        {
            // crust
            { -20.0f,  50.0f }
        },

        // rainfall: min, max
        {
            // crust
            { 5.0f,  5000.0f }
        },

        // drainage: min, max
        {
            // crust
            { 5.0f,  5000.0f }
        },

        // landscape
        {
            // mountainLandscape_t
            { 10000.0f,  20.0f },
            0.5f,

            // basinLandscape_t
            { 5000.0f,  20.0f },
            0.5f,

            // rangeLandscape_t
            { 3000.0f,  20000.0f },
            0.5f,

            // ravineLandscape_t
            { 300.0f,  10000.0f },
            0.5f,

            // lakeLandscape_t
            { 500.0f,  pnp::CC_WATER },
            0.5f,

            // riverLandscape_t
            { 100.0f,  pnp::CC_WATER,  1.0f,  pnp::D_NONE },
            0.5f
        },

        // living
        {
            // space
            {
                { pnp::CL_NONE, 0.0f },
            },
            // atmosphere
            {
                { pnp::CL_NONE, 0.0f },
            },
            // crust
            {
                // #i �� 1 ��. �� ������� � ���-������� (������) ������� �����
                //     2 ���� ��������, ���������� �������� 740 ��� �������.
                // #i ����� ������� ���� ������� ����� 149 ��� ��. ��.
                // #i ������ ������� Dungeon Crawl ~70 ��, ��� � ~70 ���
                //    ������ �������.
                { pnp::CL_WORKER_ANT,  static_cast< real_t >(2e9 * (150e6 / 10.0) / 70.0) },
                { pnp::CL_NONE, 0.0f },
            },
            // mantle
            {
                { pnp::CL_NONE, 0.0f },
            },
            // core
            {
                { pnp::CL_NONE, 0.0f },
            }
        }
#endif
    };



    topology.aboutPlanet = aboutPlanet;

    /* - ������ �������� � ������������. ������������� ������ ��� ������ init().
    //topology.aboutComponent = aboutComponent;
    std::memcpy( topology.aboutComponent,  pnp::aboutComponent,  sizeof( pnp::aboutComponent ) );
    topology.component = component;    
    //topology.aboutLiving = aboutLiving;
    std::memcpy( topology.aboutLiving,  pnp::aboutLiving,  sizeof( pnp::aboutLiving ) );
    topology.living = living;
    */



    // �������������� ������ �������
    std::cout << std::endl;

    // ����� ������� � ����, � ����
    // # ������ ����������� � shared_ptr, �.�. �� ����� ���������� ���
    //   ��������� ������� ������ �������.
    std::shared_ptr< pep::Engine >  engine( new pep::Engine( HALF_DAY ) );
    heap->engine.planet.insert( engine );

    engine->incarnate( planet );
    // #! planet = nullptr
    engine->init();


    // ����������� ������ ������� �� ������� �� ������� �������
    // @todo fine ������� �����������. �����?
    ess->addListenerStarSystem( engine, ess, engine );

    // ����������� ������ ������� ������� �� ������� �� �������
    engine->addListenerPlanet( ess, engine, ess );



    // ������� ���������
    pniop::TextVisual::option_t  o;
#if 0
    // @todo o[ "only" ] = ".aboutPlanet";
#endif

    pniop::TextVisual  visual( std::cout, o );
    visual << "\n\n" << *engine->portulan();
    

    // ������� ������ ���������
    pniop::SnapshotVTK  snapshot( engine->portulan() );
#if defined COMPONENT_SNAPSHOT_VISUALTEST && defined COMPONENT_DUNGEONCRAWL_PORTE
    snapshot.component();
#endif
#if defined TEMPERATURE_SNAPSHOT_VISUALTEST && defined TEMPERATURE_DUNGEONCRAWL_PORTE
    snapshot.temperature();
#endif
#if defined SURFACE_TEMPERATURE_SNAPSHOT_VISUALTEST && defined SURFACE_TEMPERATURE_DUNGEONCRAWL_PORTE
    snapshot.surfaceTemperature();
#endif
#if defined RAINFALL_SNAPSHOT_VISUALTEST && defined RAINFALL_DUNGEONCRAWL_PORTE
    snapshot.rainfall();
#endif
#if defined DRAINAGE_SNAPSHOT_VISUALTEST && defined DRAINAGE_DUNGEONCRAWL_PORTE
    snapshot.drainage();
#endif
#if defined LANDSCAPE_SNAPSHOT_VISUALTEST && defined LANDSCAPE_DUNGEONCRAWL_PORTE
    snapshot.landscape();
#endif
#if defined ILLUMINANCE_SNAPSHOT_VISUALTEST && defined ILLUMINANCE_DUNGEONCRAWL_PORTE
    snapshot.illuminance();
#endif
#if defined BIOME_SNAPSHOT_VISUALTEST && defined BIOME_DUNGEONCRAWL_PORTE
    snapshot.biome();
#endif
#if defined LIVING_SNAPSHOT_VISUALTEST && defined LIVING_DUNGEONCRAWL_PORTE
    snapshot.living();
#endif


#if 0
    const int PULSE = 1;
    int age = 0;
    while ( true ) {
        std::cout << "������� " << age << std::endl;

        std::cout << std::endl;

        visual.wait();

        // ��������� �������� ��� ��������
#if 1
        *engine << PULSE;
#else
        *engine( PULSE );
#endif

        age += PULSE;

        visual << planet;

    } // while
#endif

}
