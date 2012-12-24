#include "../include/StdAfx.h"
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


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pes = porte::world::dungeoncrawl::starsystem::l0;
namespace pnios = portulan::io::world::dungeoncrawl::starsystem::l0;

namespace pnp = portulan::world::dungeoncrawl::planet::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;
namespace pniop = portulan::io::world::dungeoncrawl::planet::l0;


// # Соберём все движки в структуру, чтобы до завершения работы программы
//   ни один из них не был уничтожен (спасибо, std::shared_ptr).
// @todo fine Переписать через std::unique_ptr?
typedef struct {
    std::set< pep::Engine::Ptr >  planet;
    std::set< pes::Engine::Ptr >  starSystem;
} heapEngine_t;
static heapEngine_t heapEngine;



static void wrapPlanet( heapEngine_t*, pes::Engine::Ptr, const pns::aboutPlanet_t& );


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
* Визуальное тестирование для проекта 'Porte'.
* Пример процесса изменения планеты.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace tc = typelib::constant::physics;


    // Приоритет определяет размер лог-файла
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
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );




// звёздная система
// @todo fine Заполнить данными из др. звёздных систем >
//       http://ru.wikipedia.org/wiki/%D0%A1%D0%BF%D0%B8%D1%81%D0%BE%D0%BA_%D1%8D%D0%BA%D0%B7%D0%BE%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82_%D0%B2_%D0%BE%D0%B1%D0%B8%D1%82%D0%B0%D0%B5%D0%BC%D0%BE%D0%B9_%D0%B7%D0%BE%D0%BD%D0%B5
#if 1
// @todo fine В звёздной системе работать с двойной точностью.
//       NVIDIA 8800GTS работает только с real_t.

    // Инициализируем движок звёздной системы
    // # Инициализация происходит частичная, чтобы в процессе формирования
    //   портулана звёзной системы можно было привязывать к элементам
    //   другие движки, подписывая их на события друг друга. См. wrapPlanet().
    // 'timestep' влияет на точность рассчётов (больше - ниже).
    // Может быть задействована вместе с 'PULSE'.
    static const double timestep = HOUR;
    // # Движок оборачиваем в shared_ptr, т.к. он будет отдаваться как
    //   слушатель событий другим движкам.
    pes::Engine::Ptr  engine( new pes::Engine( timestep ) );
    heapEngine.starSystem.insert( engine );


    typedef pns::Portulan  starSystem_t;
    auto starSystem = std::unique_ptr< starSystem_t >( new starSystem_t() );
    pns::topology_t& topology = starSystem->topology().topology();

    static const pns::aboutStarSystem_t aboutStarSystem = {
        // size
        // ~ Размер ~ 10000 а.е. ~ 1.5e12 км
        { 1.5e15, 1.5e15, 1.5e15 }
    };

    // где расположен пояс астероидов
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    //static const pns::real_t asteroidOrbit = 1.49598261e11 * 2.8;
    static const pns::real_t asteroidOrbit = 1.49598261e11;

    topology.aboutStarSystem = aboutStarSystem;



    // звёзды
    // # звезда - центр координат
    pns::star_t star = {};
    auto& tsc = topology.star.content;
    size_t countStar = 0;

#if 1
    // Звезда I
    // @source http://ru.wikipedia.org/wiki/%D0%A1%D0%BE%D0%BB%D0%BD%D1%86%D0%B5
#if 1
    {
        static const pns::aboutStar_t star = {
            // uid
            countStar + 1,
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

    // Звезда II
#if 0
    {
        static const pns::aboutStar_t star = {
            // uid
            countStar + 1,
            // mass
            1.9891e30,
            // radius
            6.9551e8,
            // temperature,
            1.5e6,
            // luminousIntensity
            3e27,
            // coord
            { 0, asteroidOrbit * 1.5, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { -15000, 0, 0 }
        };
        tsc[ countStar ] = star;
        ++countStar;
    }
#endif

    // Звезда III
#if 0
    {
        static const pns::aboutStar_t star = {
            // uid
            countStar + 1,
            // mass
            1.9891e30,
            // radius
            6.9551e8,
            // temperature,
            1.5e6,
            // luminousIntensity
            3e27,
            // coord
            { 0, -asteroidOrbit * 1.5, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 15000, 0, 0 }
        };
        tsc[ countStar ] = star;
        ++countStar;
    }
#endif

    // завершаем список пустотой
    static const pns::aboutStar_t STAR_END_LIST = {};
    tsc[ countStar ] = STAR_END_LIST;

#endif // звёзды



    // планеты
    pns::planet_t planet = {};
    auto& tpc = topology.planet.content;
    size_t countPlanet = 0;

#if 1
    // Меркурий
    // @source http://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%80%D0%BA%D1%83%D1%80%D0%B8%D0%B9_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 0
    {
        static const pns::uid_t uid = 1;
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
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
            { 0, 47870, 0 }
        };
        tpc[ countPlanet ] = planet;
        ++countPlanet;
    }
#endif

    // Венера
    // @source http://ru.wikipedia.org/wiki/%D0%92%D0%B5%D0%BD%D0%B5%D1%80%D0%B0_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 0
    {
        static const pns::uid_t uid = 2;
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // mass
            4.8685e24,
            // radius
            6.0518e6,
            // coord
            { 1.07476259e11, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0, 35020, 0 }
        };
        tpc[ countPlanet ] = planet;
        ++countPlanet;
    }
#endif

    // Земля
    // @source http://ru.wikipedia.org/wiki/%D0%97%D0%B5%D0%BC%D0%BB%D1%8F
#if 1
    {
        static const pns::uid_t uid = 3;
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
            // mass
            5.9736e24,
            // radius
            6.3568e6,
            // coord
            { 1.49598261e11, 0, 0 },
            // rotation
            { 0, 0, 0 },
            // force
            { 0, 0, 0 },
            // velocity
            { 0, 29783, 0 },
            // axilTilt
            23.44,
            // rotationPeriod
            0.99726968 * 24 * 60 * 60
        };
        tpc[ countPlanet ] = planet;
        ++countPlanet;

        // тут же оборачиваем планету в свой движок
        wrapPlanet( &heapEngine, engine, planet );
    }
#endif


    // Марс
    // @source http://ru.wikipedia.org/wiki/%D0%9C%D0%B0%D1%80%D1%81_%28%D0%BF%D0%BB%D0%B0%D0%BD%D0%B5%D1%82%D0%B0%29
#if 0
    {
        static const pns::uid_t uid = 4;
        static const pns::aboutPlanet_t planet = {
            // uid
            uid,
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
            { 0, 24130, 0 }
        };
        tpc[ countPlanet ] = planet;
        ++countPlanet;
    }
#endif

    // завершаем список пустотой
    static const pns::aboutPlanet_t PLANET_END_LIST = {};
    tpc[ countPlanet ] = PLANET_END_LIST;

#endif // планеты



    // астероиды
    // @source http://ru.wikipedia.org/wiki/%D0%90%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4
    // @source http://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%8F%D1%81_%D0%B0%D1%81%D1%82%D0%B5%D1%80%D0%BE%D0%B8%D0%B4%D0%BE%D0%B2
    static const size_t N_ASTEROID = 100;
    assert( (N_ASTEROID <= pns::ASTEROID_COUNT) &&
        "Количество астероидов превышает зарезервированный для них объём." );
    size_t countAsteroid = 0;

#if 1
    pns::asteroid_t asteroid = {};
    auto& tac = topology.asteroid.content;
    {
        // не случайный генератор случ. чисел: полезно каждый
        // раз видеть одинаковые (предсказуемые) результаты
        static const size_t SEED = 12345;
        typelib::Random< size_t >  seed( 0, 1000000000, SEED );

        typelib::Random< pns::real_t >  averageRadius( 100, 300000, seed.next() );
        typelib::Random< pns::real_t >  proportionRadius( 0.5, 1.5, seed.next() );
        typelib::Random< pns::real_t >  density( 2000, 4000, seed.next() );

        typelib::Random< pns::real_t >  distance(
            asteroidOrbit * 0.9, asteroidOrbit * 1.1, seed.next()
        );
        typelib::Random< pns::real_t >  angle( 0.0, M_PI * 2.0, seed.next() );

        static const pns::real_t velocityOrbit = 17500;
        typelib::Random< pns::real_t >  velocity(
            velocityOrbit * 0.95, velocityOrbit * 1.05, seed.next()
        );

        size_t i = 0;
        for ( ; i < N_ASTEROID; ++i) {
            // # Принимаем за эллипсоид.
            const pns::real_t ar = averageRadius.next();
            const pns::real_t rx = ar * proportionRadius.next();
            const pns::real_t ry = ar * proportionRadius.next();
            const pns::real_t rz = ar * proportionRadius.next();

            const pns::real_t volume = 4.0 / 3.0 * M_PI * rx * ry * rz;
            const pns::real_t _density = density.next();
            const pns::real_t mass = volume * _density;

            const pns::real_t d  = distance.next();
            const pns::real_t da = angle.next();
            const pns::real_t cx = d * std::sin( da );
            const pns::real_t cy = d * std::cos( da );
            const pns::real_t cz = 0;

            // скорости - перпендикулярны силе от центра звёздной системы
            const pns::real_t _velocity = velocity.next();
            const pns::real_t vx = _velocity * std::sin( da - M_PI_2 ) * 0.0;
            const pns::real_t vy = _velocity * std::cos( da - M_PI_2 ) * 0.0;
            const pns::real_t vz = 0.0;

            const pns::aboutAsteroid_t asteroid = {
                // uid
                i + 1,
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
                // memoryEvent
                { 0, {} }
            };
            tac[ countAsteroid ] = asteroid;
            ++countAsteroid;

        } // for (size_t i = 0; i < N; ++i)

        // завершаем список пустотой
        static const pns::aboutAsteroid_t ASTEROID_END_LIST = {};
        tac[ countAsteroid ] = ASTEROID_END_LIST;
    }
#endif // астероиды



    // @test
    std::cout << "asteroid" <<
        "  " << countAsteroid << " / " << pns::ASTEROID_COUNT << "u" <<
        "  ~ " << sizeof( pns::asteroid_t ) / 1024 << "Кб" <<
        std::endl << "planet" <<
        "  " << countPlanet << " / " << pns::PLANET_COUNT << "u" <<
        "  ~ " << sizeof( pns::planet_t ) / 1024 << "Кб" <<
        std::endl << "star" <<
        "  " << countStar << " / " << pns::STAR_COUNT << "u" <<
        "  ~ " << sizeof( pns::star_t ) / 1024 << "Кб" <<
        std::endl <<
    std::endl;



    // Воплощаем звёздную систему
    engine->incarnate( std::move( starSystem ) );
    // #! starSystem = nullptr

#endif // звёздная система



    // Покажем результат для звёздной системы
    pnios::VolumeVTKVisual::option_t  o;
    o[ "extent" ] = engine->extent();
    o[ "asteroid-size-point" ] = 2;
    o[ "planet-size-point" ] = 5;
    o[ "star-size-point" ] = 10;
    o[ "auto-scale-camera" ] = false;
    o[ "without-clear" ] = false;
    o[ "size-window" ] = 950;

    pnios::VolumeVTKVisual  visual( o );
    visual << *engine->portulan();
    

    // # Учитываем инициал. движка звёздной системы, а именно - 'timestep'.
    // 'PULSE' влияет на кол-во отрисовок (больше пульс - меньше кадров).
    // Может быть задействована вместе с 'timestep'.
    // Движок честно считает 'PULSE' кадров с шагом 'timestep' и только
    // после - показывает картинку. Др. словами, реальное время отображаемых
    // кадров = timestep * PULSE.
    // @example timestep = HOUR,  PULSE = 365 * 24 - Земля будет оставаться
    //          почти неподвижной, т.к. её период обращения ~ 365 дней.
    static const int PULSE = 24;

    // запускаем мир
    visual.wait( engine.get(), PULSE, 1 );




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()










/**
* Подключает к планете звёздной системы движок.
*
* # Возьмём данные о планете в формате звёздной системы и передадим их
*   движку планеты.
* # Подключим движок планеты как слушателя пульсов и событий движка
*   звёздной системы.
* # Подключим движок звёздной системы как слушателя событий планеты. Например,
*   движок звёздной системы должен знать о разрушении / расколе планеты.
*/
void wrapPlanet(
    heapEngine_t*              heapEngine,
    pes::Engine::Ptr           ess,
    const pns::aboutPlanet_t&  pss
) {
    assert( heapEngine );

#if 1
    // расширяем топологию планеты, опираясь на дынные 'planetStarSystem'
    /* - Нет. Можем оживить любую планету.
    static const std::string charUID = "earth";
    assert( (strcmp(planetStarSystem.charUID, charUID.c_str()) == 0)
        && "Ожидалось получить планету с другим UID" );
    */

    typedef pnp::Portulan  planet_t;
    auto planet = std::unique_ptr< planet_t >( new planet_t() );
    pnp::topology_t& topology = planet->topology().topology();

    // # Мир не большой - работаем с 'float'. См. также структуры и
    //   planet::Engine.
    typedef float  real_t;

    // общая информация об области планеты
    // @source http://ru.wikipedia.org/wiki/%D0%97%D0%B5%D0%BC%D0%BD%D0%B0%D1%8F_%D0%BA%D0%BE%D1%80%D0%B0
    const real_t radiusCrust = pss.radius;
    const real_t radiusAtmosphere = radiusCrust + 200.0f * 1000.0f;
    const real_t halfSize = radiusAtmosphere;
    const real_t radiusMantle = radiusCrust - 200.0f * 1000.0f;
    const real_t radiusCore = radiusCrust * 0.4f;

    // @todo = f( данные планеты из звёздной системы )
    const real_t massAtmosphere = static_cast< real_t >( 5e18 );
    const real_t massCrust = static_cast< real_t >( 4e22 );
    const real_t massMantle = static_cast< real_t >( 4e24 );
    const real_t massCore = static_cast< real_t >( 3e24 );

    static const pnp::aboutPlanet_t aboutPlanet = {
#if 0
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
            { tc::CK + 1.0f,  tc::CK + 1.0f },
            // atmosphere
            {   20.0f,  tc::CK + 2.0f },
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
                // #i На 1 кв. км саванны в Кот-д’Ивуаре (Африка) обитает почти
                //     2 млрд муравьёв, образующих примерно 740 тыс колоний.
                // #i Общая площадь суши планеты Земля 149 млн кв. км.
                // #i Размер муравья Dungeon Crawl ~70 см, что в ~70 раз
                //    больше земного.
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

    /* - Память выделена в конструкторе. Инициализация пройдёт при вызове init().
    //topology.aboutComponent = aboutComponent;
    std::memcpy( topology.aboutComponent,  pnp::aboutComponent,  sizeof( pnp::aboutComponent ) );
    topology.component = component;    
    //topology.aboutLiving = aboutLiving;
    std::memcpy( topology.aboutLiving,  pnp::aboutLiving,  sizeof( pnp::aboutLiving ) );
    topology.living = living;
    */

    // будем считать и день, и ночь
    // # Движок оборачиваем в shared_ptr, т.к. он будет отдаваться как
    //   слушатель событий другим движкам.
    pep::Engine::Ptr  enginePlanet( new pep::Engine( HALF_DAY ) );
    heapEngine->planet.insert( enginePlanet );


#if 0
    // состав планеты (компоненты)
    component_t component;
    {
        const auto fn = [] (
            portulan::planet::componentCell_t&  cc,
            const typelib::coord_t&  c,
            const portulan::planet::aboutPlanet_t&  aboutPlanet
        ) -> void {
            using namespace portulan::planet;
            // # Распределяем компоненты равномерно:
            //   - по ячейкам планеты
            //   - по ячейкам атмосферы
            // # Область планеты вне атмосферы содержит вакуум (CC_VACUUM)
            const real_t scale =
                aboutPlanet.sizeArea / static_cast< real_t >( portulan::GRID_SX );
            const real_t d = c.distance< real_t >() * scale;

            const auto fnFill = [ &cc ] ( const componentAll_t& ca ) -> void {
                using namespace portulan::planet;
                auto dtr = std::begin( cc );
                for (code_t code = 0; code < CC_count; ++code) {
                    const auto count = ca[ code ].count;
                    if (count > 0.0f) {
                        dtr->code = code;
                        dtr->count = count;
                        ++dtr;
                    }
                }
                // дозаполняем нулями
                for ( ; dtr != std::end( cc ); ++dtr) {
                    dtr->code = static_cast< code_t >( 0 );
                    dtr->count = 0.0f;
                }
            };

            // планета?
            if (d <= aboutPlanet.radiusPlanet) {
                fnFill( aboutPlanet.componentPlanet );
                return;
            }

            // атмосфера?
            if (d <= (aboutPlanet.sizeArea / 2.0f) ) {
                fnFill( aboutPlanet.componentAtmosphere );
                return;
            }

            // пустое пространство, заполняем вакуумом
            for (auto dtr = std::begin( cc ); dtr != std::end( cc ); ++dtr) {
                dtr->code = CC_VACUUM;
                dtr->count = 1.0f;
            }

        }; // const auto fn = [] ( ...

        co::component(
            component,
            aboutPlanet,
            fn
        );
    }




    // информация об особях живого мира, встречающихся в области планеты
    aboutLiving_t aboutLiving;
    {
        co::aboutLiving( aboutLiving );
    }




#if 0

    // живой мир
    living_t living;
    {
        // муравей
        // # Не делим на матку, фуражиров, воинов и т.п. - при моделировании
        //   на уровне планеты при тек. мощностях подобное деление - роскошь.
        {
            topology_t::aboutOneLiving_t  aboutOneLiving;

            // время жизни муравья, пульсов (1 пульс = 1 год)
            const size_t lifetime = 2.0;

            // масса муравья по жизненным циклам, кг
            // @source http://bolshoyvopros.ru/questions/53986-skolko-vesit-muravej.html
            // функция ниже равномерно распределит массу по LIFE_CYCLE между
            // заданными значениями
            const double massBurn = 0.001 / 1000.0;
            const double massOld = 0.008 / 1000.0;
            const std::vector< double >  mass = ba::list_of
                ( massBurn )
                ( massOld )
            ;

            /* ?
               В человеке, весящем 70 килограммов, содержится:
                 - 45 литров воды
                 - достаточно извести, чтобы побелить курятник
                 - фосфора, которого хватит на 2200 спичек
                 - жира примерно на 70 кусков мыла
                 - железа на двухдюймовый гвоздь
                 - углерода на 2000 карандашей
                 - одна ложка магния
               (c) Эрленд Лу "Наивно. Супер."
            */
            // какие вещества содержит муравей
            //   - из каких хим. веществ состоит особь
            //   - с какой скоростью сжигаются выводятся хим. вещества для
            //     поддержания жизни особи
            // @see topology_t::chemicalComposition_t
            // @source http://antclub.org/morphology/anatomy
            // @source http://antclub.ru/muravi/stroenie-muravya
            // @source http://entomolog.su/kutikula-i-ee-chimicheskiy-sostav
            // @source http://medbiol.ru/medbiol/botanica/0015e3fc.htm
            // @source http://ru.wikipedia.org/wiki/%D0%A5%D0%B8%D0%BC%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B9_%D1%81%D0%BE%D1%81%D1%82%D0%B0%D0%B2_%D0%BA%D0%BB%D0%B5%D1%82%D0%BA%D0%B8
            const std::map< int, double >  chemicalComposition = ba::map_list_of
                // углеводы
                ( CC_CARBOHYDRATE,  1.0 - 0.2 - 0.3 - 0.05 )
                // жиры
                ( CC_FAT,  0.05 )
                // белки
                ( CC_PROTEIN,  0.3 )
                // вода
                ( CC_WATER,  0.2 )
            ;

            // метаболизм муравья
            topology_t::aboutOneLiving_t::metabolism_t  metabolism[ topology_t::LC_count ];

            // нормы для человека (100 кг):
            //   - кислород: 10 л / мин = 10 * 1.43 г ~ 14.3 г / мин
            //   - белки: 1.5 г / кг веса / сутки
            //   - жиры: 120 г / сутки
            //   - углеводы: 500 г / сутки
            //   + углекислый газ: 5 л / мин = 5 * 1.966 г ~ 9.83 г / мин
            //   + испражнения: ~200 г / день
            // указываем значение на 1 кг и передаём функции-заполнителю
            // массы от рождения до смерти особи
            const double oxygenNeed = 14.3 / 1000.0 * MINUTE_IN_YEAR / 100.0;
            const double proteinNeed = 1.5 / 1000.0 * DAY_IN_YEAR;
            const double fatNeed = 120.0 / 1000.0 * DAY_IN_YEAR;
            const double carbohydrateNeed = 500.0 / 1000.0 * DAY_IN_YEAR;
            const std::map< int, double >  chemicalNeed = ba::map_list_of
                // углеводы
                ( CC_CARBOHYDRATE,  carbohydrateNeed )
                // жиры
                ( CC_FAT,  fatNeed )
                // кислород
                ( CC_OXYGEN,  oxygenNeed )
                // белки
                ( CC_PROTEIN,  proteinNeed )
            ;

            const double carbonAcidWaste = 9.83 / 1000.0 * MINUTE_IN_YEAR / 100.0;
            const double excrementWaste = 200.0 / 1000.0 * DAY_IN_YEAR / 100.0;
            const std::map< int, double >  chemicalWaste = ba::map_list_of
                // углекислый газ
                ( CC_CARBON_ACID,  carbonAcidWaste )
                // испражнения
                ( CC_EXCREMENT,    excrementWaste )
            ;

            // нормы для человека (100 кг):
            //   - требуемая энергия: не требуется (всё может быть получено из пищи)
            //   - излучаемая энергия: нет (полагаем, что вся полученная
            //     энергия расходуется на поддержание жизни)
            const std::vector< double >  energyNeed;
            const std::vector< double >  energyWaste;

            co::metabolism< GRID, GRID, GRID >(
                metabolism,
                chemicalNeed, chemicalWaste,
                energyNeed, energyWaste,
                lifetime,
                massBurn, massOld
            );


            // условия выживания
            topology_t::aboutOneLiving_t::survivor_t  survivor;

            // среды, в которых может обитать особь
            const std::vector< size_t > environment = ba::list_of
                // муравьи могут жить на границе твёрдого тела и газа (на поверхности почвы)
                ( topology_t::Environment::GAS | topology_t::Environment::SOLID )
                // муравь могут жить в самой почве (муравейники, норы)
                ( topology_t::Environment::SOLID )
            ;
            // температуры комфорта / выживания делаем едиными для всех возрастов
            const std::pair< double, double >  comfortTemperature( 15, 30 );
            const std::pair< double, double >  limitTemperature( 0, 50 );
            co::survivor< GRID, GRID, GRID >(
                survivor,
                environment,
                comfortTemperature, limitTemperature
            );


            const double immunity = 0.9;
            co::aboutOneLiving< GRID, GRID, GRID >(
                aboutOneLiving,
                lifetime,
                massBurn, massOld,
                immunity,
                chemicalComposition, metabolism, survivor
            );

            std::cout << std::endl << "*Муравей*" << std::endl << aboutOneLiving << std::endl;

            
            // определяем, в каких ячейках области планеты будут жить муравьи
            const auto fnLiving = [] (
                size_t pulse,
                const typelib::coord_t& c,
                const topology_t::aboutOneLiving_t& l
            ) -> double {
                // @todo Инициализировать в зависимости от среды обитания 'sp'.
                // добавляем только взрослых особей
                // (для муравьёв это 1-й пульс)
                const double count = (pulse == 1) ? 1000 : 0;
                return count;
            };

            co::living< GRID, GRID, GRID >(
                living,
                CL_ANT,
                // кол-во муравьёв в ячейке области планеты
                fnLiving,
                // информация об особи
                aboutOneLiving
            );

        } // муравей

    } // topology_t::living_t




    // @todo компоненты

    // Перечисление UID компонентов мира
    enum UID_COMPONENT {
        // первый UID компонента идёт после всех живых особей: т.о. мы
        // сможем отделить живых особей от компонентов без необходимости
        // вводить доп. структуры
        UIDC_UNDEFINED = topology_t::SPECIMEN_COUNT,
        // падь (пища насекомых)
        UIDC_HONEYDEW,
        UIDC_count
    };

    topology_t::component_t component;
    {
        component.content[0][0] = 0.0f;
    }





    // температура
    topology_t::temperature_t temperature;
    {
        // температура к центру планеты - растёт, отдаляясь от поверхности -
        // уменьшается, на поверхности планеты - достигает ~20 C
        //const size_t TG = topology_t::TEMPERATURE_GRID;
        const auto fnTemperature = [] (
            const typelib::coord_t& c,
            double radiusPlanet,
            double radiusAtmosphere
        ) -> double {
            /* - Работаем только с сеткой ячеек. См. ниже.
            const double dist = (c * scale * 1.25).distance< double >();
            const double d = dist - radiusPlanet;
            const int sign = boost::math::sign( d );
            double t = ( std::pow( ((sign < 0) ? -d : d),  1.0 / 2.0 ) + 1.0) * 20.0
                + typelib::constant::physics::CK * 0;
            if (sign >= 0) {
                t = -t;
            }
            */
            const size_t dist = static_cast< size_t >( c.distance< real_t >() + 0.5f );
            // табличная функция температуры в зависимости от расстояния до центра планеты,
            // градусы Цельсия
            static const size_t NC = 23;
            assert( (dist <= static_cast< double >( NC )) &&
                "Табличная функция содержит меньше ожидаемого кол-ва значений температур." );
            const double tc[ NC ] = {
                // 22, самая дальняя от центра планеты точка
                -100, -90, -80, -70, -60, -50, -40, -30, -20, -10,
                // 12, поверхность планеты
                20,
                // 2, кора планеты
                100,  180,  324,  583, 1049, 1888, 3398, 6116, 6300, 6500,
                // 1
                6700,
                // 0, центр планеты
                7000
            };
            const size_t index = NC - dist;
            // возвращаем температуру в Кельвинах
            double t = tc[ index ] - typelib::constant::physics::CK;
            const bool clamped =
                typelib::clampr( t, typelib::constant::physics::CK, typelib::constant::physics::Tp );
            assert( !clamped &&
                "Температура превышает возможные физические границы." );
            return t;
        };

        co::temperature< GRID, GRID, GRID >(
            temperature,
            fnTemperature,
            radiusPlanet,
            radiusAtmosphere
        );

    }




    // @todo осадки
    topology_t::precipitations_t precipitations;
    {
        static const size_t PG = topology_t::PRECIPITATIONS_GRID;
        std::fill_n( precipitations.content,  PG * PG * PG,  0.0f );
    }





    co::planet(
        planet.topology(),
        atmosphere, crust,
        living,
        // @todo mantle, core,  ...
        temperature,
        precipitations
    );

#endif

#endif



    // инициализируем движок планеты
    std::cout << std::endl;
    enginePlanet->incarnate( std::move( planet ) );
    // #! planet = nullptr
    enginePlanet->init();


    // подписываем движок планеты на события от звёздной системы
    // @todo fine Слишком многословно. Проще?
    ess->addListenerStarSystem( enginePlanet, ess, enginePlanet );


    // подписываем движок звёздной системы на события от планеты
    enginePlanet->addListenerPlanet( ess, enginePlanet, ess );



    // покажем результат
    pniop::TextVisual::option_t  o;
#if 0
    o[ "only" ] = ".aboutPlanet";
#endif

    pniop::TextVisual  visual( std::cout, o );
    visual << "\n\n" << *enginePlanet->portulan();
    

    // Сделаем снимок топологии
    pniop::SnapshotVTK  snapshot( enginePlanet->portulan() );
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
        std::cout << "Возраст " << age << std::endl;

        std::cout << std::endl;

        visual.wait();

        // одинаково работают оба варианта
#if 1
        *engine << PULSE;
#else
        *engine( PULSE );
#endif

        age += PULSE;

        visual << planet;

    } // while
#endif

#endif

}
