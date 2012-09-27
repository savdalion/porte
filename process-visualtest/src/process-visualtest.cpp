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



/**
* Визуальное тестирование для проекта 'Porte'.
* Пример процесса изменения планеты.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;
    using namespace portulan::planet;

    //namespace co = portulan::command::planet;
    namespace ps  = portulan::planet::set;
    namespace pd = portulan::planet::set::dungeoncrawl;
    namespace tc  = typelib::constant::physics;


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


    typedef Portulan  planet_t;
    planet_t planet;
    ps::topology_t& topology = planet.topology().topology();


    // общая информация об области планеты
    // @source http://ru.wikipedia.org/wiki/%D0%97%D0%B5%D0%BC%D0%BD%D0%B0%D1%8F_%D0%BA%D0%BE%D1%80%D0%B0
    const float radiusCrust = 7000.0f * 1000.0f;
    const float radiusAtmosphere = radiusCrust + 200.0f * 1000.0f;
    const float halfSize = radiusAtmosphere;
    const float radiusMantle = radiusCrust - 200.0f * 1000.0f;
    const float radiusCore = 3000.0f * 1000.0f;

    const float massAtmosphere = static_cast< cl_float >( 5e18 );
    const float massCrust = static_cast< cl_float >( 4e22 );
    const float massMantle = static_cast< cl_float >( 4e24 );
    const float massCore = static_cast< cl_float >( 3e24 );

    static const pd::aboutPlanet_t aboutPlanet = {
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
                { pd::CC_NONE, 0.0f },
            },
            // atmosphere
            {
                { pd::CC_AIR,        100.0f / 100.0f },
                { pd::CC_NONE, 0.0f },
            },
            // crust
            {
                { pd::CC_AIR,          3.0f / 100.0f },
                { pd::CC_BARREN_SOIL, 14.0f / 100.0f },
                { pd::CC_RICH_SOIL,    1.0f / 100.0f },
                { pd::CC_SAND,        10.0f / 100.0f },
                { pd::CC_ROCK,        20.0f / 100.0f },
                { pd::CC_BOULDER,      4.0f / 100.0f },
                { pd::CC_WATER,       47.0f / 100.0f },
                { pd::CC_SPARSE,       0.5f / 100.0f },
                { pd::CC_NONE, 0.0f },
            },
            // mantle
            {
                { pd::CC_AIR,          1.0f / 100.0f },
                { pd::CC_BARREN_SOIL, 12.0f / 100.0f },
                { pd::CC_SAND,        20.0f / 100.0f },
                { pd::CC_ROCK,        60.0f / 100.0f },
                { pd::CC_WATER,        2.0f / 100.0f },
                { pd::CC_SPARSE,       5.0f / 100.0f },
                { pd::CC_NONE, 0.0f },
            },
            // core
            {
                { pd::CC_AIR,          0.1f / 100.0f },
                { pd::CC_ROCK,        90.0f / 100.0f },
                { pd::CC_WATER,        0.9f / 100.0f },
                { pd::CC_SPARSE,       9.0f / 100.0f },
                { pd::CC_NONE, 0.0f },
            }
        },

        // temperature: center, surface
        {
            // space
            {    1.0f,              1.0f },
            // atmosphere
            {   20.0f - tc::CK,     2.0f },
            // crust
            { 1000.0f - tc::CK,    20.0f - tc::CK },
            // mantle
            { 4000.0f - tc::CK,  1000.0f - tc::CK },
            // core
            { 6000.0f - tc::CK,  4000.0f - tc::CK }
        },

        // surfaceTemperature: equator, pole
        {
            // crust
            { -50.0f - tc::CK,  50.0f - tc::CK }
        },

        // rainfall: min, max
        {
            // crust
            { 50.0f,  5000.0f }
        },

        // drainage: min, max
        {
            // crust
            { 50.0f,  5000.0f }
        },

        // living
        {
            // space
            {
                { pd::CL_NONE, 0.0f },
            },
            // atmosphere
            {
                { pd::CL_NONE, 0.0f },
            },
            // crust
            {
                // #i На 1 кв. км саванны в Кот-д’Ивуаре (Африка) обитает почти
                //     2 млрд муравьёв, образующих примерно 740 тыс колоний.
                // #i Общая площадь суши планеты Земля 149 млн кв. км.
                // #i Размер муравья Dungeon Crawl ~70 см, что в ~70 раз
                //    больше земного.
                { pd::CL_WORKER_ANT,  static_cast< float >(2e9 * (150e6 / 10.0) / 70.0) },
                { pd::CL_NONE, 0.0f },
            },
            // mantle
            {
                { pd::CL_NONE, 0.0f },
            },
            // core
            {
                { pd::CL_NONE, 0.0f },
            }
        }

    };



    topology.aboutPlanet = aboutPlanet;

    // @test
    const auto& testC = topology.aboutPlanet.component;
    const auto& testL = topology.aboutPlanet.living;
    const auto& testT = topology.aboutPlanet.temperature;
    const auto& testST = topology.aboutPlanet.surfaceTemperature;
    const auto& testR = topology.aboutPlanet.rainfall;
    const auto& testD = topology.aboutPlanet.drainage;

    /* - Память выделена в конструкторе. Инициализация пройдёт при вызове init().
    //topology.aboutComponent = aboutComponent;
    std::memcpy( topology.aboutComponent,  pd::aboutComponent,  sizeof( pd::aboutComponent ) );
    topology.component = component;    
    //topology.aboutLiving = aboutLiving;
    std::memcpy( topology.aboutLiving,  pd::aboutLiving,  sizeof( pd::aboutLiving ) );
    topology.living = living;
    */

    typedef DungeonCrawl  dungeonCrawl_t;
    dungeonCrawl_t  dungeonCrawl( &planet );


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
            const float scale =
                aboutPlanet.sizeArea / static_cast< float >( portulan::GRID_SX );
            const float d = c.distance< float >() * scale;

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
            const size_t dist = static_cast< size_t >( c.distance< float >() + 0.5f );
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



    // Инициализируем область планеты
    std::cout << std::endl;
    dungeonCrawl.init();


    // Покажем результат
    namespace pio = portulan::io::planet::set::dungeoncrawl;
    pio::TextVisual::option_t  o;
#if 0
    o[ "only" ] = ".aboutPlanet";
#endif

    pio::TextVisual  visual( std::cout, o );
    visual << planet;
    

    // Сделаем снимок топологии
    pio::SnapshotVTK  snapshot( &planet );
#ifdef COMPONENT_DUNGEONCRAWL_PORTE
    snapshot.component();
#endif
#ifdef LIVING_DUNGEONCRAWL_PORTE
    snapshot.living();
#endif
#ifdef TEMPERATURE_DUNGEONCRAWL_PORTE
    snapshot.temperature();
#endif


    // @todo std::cout << std::endl << "Нажимаем 'Enter' для изменения планеты..." << std::endl << std::endl;

    const int PULSE = 1;
    int age = 0;
    while ( true ) {
        std::cout << "Возраст " << age << std::endl;

        std::cout << std::endl;

        visual.wait();

        // одинаково работают оба варианта
#if 1
        dungeonCrawl << PULSE;
#else
        dungeonCrawl( PULSE );
#endif

        age += PULSE;

        visual << planet;

    } // while




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
