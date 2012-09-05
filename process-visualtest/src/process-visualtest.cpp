#include "../include/StdAfx.h"
#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>



/**
* Визуальное тестирование для проекта 'Porte'.
* Пример процесса изменения планеты.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace co = portulan::command::planet;
    namespace ba = boost::assign;


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



    const size_t GRID = 81;
    typedef portulan::planet::Portulan< GRID, GRID, GRID >  planet_t;
    planet_t planet;


    // I. Иницализируем область планеты (атмосфера, кора, мантия, ядро)
    //    # 1 пульс = 1 год = 400 дней
    //    # 1 день = 25 часов
    const double DAY_IN_YEAR = 400;
    const double HOUR_IN_YEAR = DAY_IN_YEAR * 25;
    const double MINUTE_IN_YEAR = HOUR_IN_YEAR * 60;
    const double SECOND_IN_YEAR = MINUTE_IN_YEAR * 60;

    // размер планеты (он же - размер коры планеты), км
    const double radiusPlanet = 7000;

    typedef portulan::planet::Topology< GRID, GRID, GRID >  topology_t;


    // Перечисление кодов в группе живого мира
    enum CODE_LIVING {
        CL_UNDEFINED = 0,
        CL_ANT,
        CL_FLOWER,
        // количество кодов живого мира
        CL_count
    };



    // Перечисление кодов в группе компонентов
    enum CODE_COMPONENT {
        CC_UNDEFINED = 0,
        CC_Al,
        CC_Ar,
        CC_C,
        // углеводы
        CC_CARBOHYDRATE,
        // углекислый газ
        CC_CO2,
        // экскременты
        CC_EXCREMENT,
        // жиры
        CC_FAT,
        CC_Fe,
        CC_H,
        // вода
        CC_H2O,
        CC_Ca,
        CC_K,
        CC_Mg,
        CC_Na,
        CC_N2,
        // кислород
        CC_O2,
        // белки
        CC_PROTEIN,
        CC_Si,
        // редкие элементы
        CC_SPARSE,
        CC_Ti,
        // количество кодов компонентов
        CC_count
    };




    // атмосфера
    // внешний радиус атмосферы (он же - размер области планеты), км
    const double radiusAtmosphere = radiusPlanet + 200;
    topology_t::atmosphere_t atmosphere;
    {
        const double mass = 7e18;
        // толщина слоёв области планеты задаётся в км
        const double innerRadius = radiusPlanet;
        const double depth = radiusAtmosphere - innerRadius;
        const std::map< int, double >  chemicalSubstance = ba::map_list_of
            ( CC_N2,   77.0 / 100.0 )
            ( CC_O2,   20.0 / 100.0 )
            ( CC_Ar,   1.5 / 100.0 )
            ( CC_CO2,  0.03 / 100.0 )
            // редкие соединения (компоненты)
            ( CC_SPARSE,  0.97 / 100.0 )
            // водяной пар (в виде аэрозоли)
            ( CC_H2O,  0.5 / 100.0 )
        ;
        co::atmosphere< GRID, GRID, GRID >(
            atmosphere,
            mass, innerRadius, depth,
            chemicalSubstance
        );
    }




    // кора планеты
    topology_t::crust_t crust;
    {
        // @source http://ru.wikipedia.org/wiki/%D0%97%D0%B5%D0%BC%D0%BD%D0%B0%D1%8F_%D0%BA%D0%BE%D1%80%D0%B0
        const double mass = 3e22;
        // толщина слоёв планетарной коры задаётся в км
        const double depth = 250;
        const double innerRadius = radiusPlanet - depth;
        const std::map< int, double >  chemicalSubstance = ba::map_list_of
            ( CC_O2,  50.0 / 100.0 )
            // Si
            ( CC_Si,  25.0 / 100.0 )
            // Al
            ( CC_Al,  7.0 / 100.0 )
            // Fe
            ( CC_Fe,  5.0 / 100.0 )
            // Ca
            ( CC_Ca,  4.0 / 100.0 )
            // Na
            ( CC_Na,  3.0 / 100.0 )
            // K
            ( CC_K,   2.0 / 100.0 )
            // Mg
            ( CC_Mg,  2.0 / 100.0 )
            // H
            ( CC_H,   1.0 / 100.0 )
            // редкие (sparse)
            ( CC_SPARSE,  1.0 / 100.0 )
        ;
        co::crust< GRID, GRID, GRID >(
            crust,
            mass, innerRadius, depth,
            chemicalSubstance
        );
    }




    // живой мир
    topology_t::living_t living;
    {
        // муравей
        // # Не делим на матку, фуражиров, воинов и т.п. - при моделировании
        //   на уровне планеты при тек. мощностях подобное деление - роскошь.
        {
            topology_t::living_t::specimen_t  specimen;

            // время жизни муравья, пульсов (1 пульс = 1 год)
            const size_t lifetime = 2.0;

            // масса муравья по жизненным циклам, кг
            // @source http://bolshoyvopros.ru/questions/53986-skolko-vesit-muravej.html
            // функция ниже равномерно распределит массу по LIFE_CYCLE между
            // заданными значениями
            const double massBurn = 0.001 / 1000.0;
            const double massDie = 0.008 / 1000.0;
            const std::vector< double >  mass = ba::list_of
                ( massBurn )
                ( massDie )
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
                // вода
                ( CC_H2O,  0.2 )
                // белки
                ( CC_PROTEIN,  0.3 )
                // жиры
                ( CC_FAT,  0.05 )
                // углеводы
                ( CC_CARBOHYDRATE,  1.0 - 0.2 - 0.3 - 0.05 )
            ;

            // метаболизм муравья
            topology_t::living_t::specimen_t::metabolism_t  metabolism[ topology_t::LIFE_CYCLE ];

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
            const std::map< int, double > chemicalNeed = ba::map_list_of
                // кислород
                ( CC_O2,  oxygenNeed )
                // белки
                ( CC_PROTEIN,  proteinNeed )
                // жиры
                ( CC_FAT,  fatNeed )
                // углеводы
                ( CC_CARBOHYDRATE,  carbohydrateNeed )
            ;

            const double carbonAcidWaste = 9.83 / 1000.0 * MINUTE_IN_YEAR / 100.0;
            const double excrementWaste = 200.0 / 1000.0 * DAY_IN_YEAR / 100.0;
            const std::map< int, double > chemicalWaste = ba::map_list_of
                // углекислый газ
                ( CC_CO2,  carbonAcidWaste )
                // испражнения
                ( CC_EXCREMENT,  excrementWaste )
            ;

            // нормы для человека (100 кг):
            //   - требуемая энергия: не требуется (всё может быть получено из пищи)
            //   - излучаемая энергия: нет (полагаем, что вся полученная
            //     энергия расходуется на поддержание жизни)
            const std::vector< double > energyNeed;
            const std::vector< double > energyWaste;

            co::metabolism< GRID, GRID, GRID >(
                metabolism,
                chemicalNeed, chemicalWaste,
                energyNeed, energyWaste,
                lifetime,
                massBurn, massDie
            );


            // условия выживания
            topology_t::living_t::specimen_t::survivor_t  survivor;

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
            co::specimen< GRID, GRID, GRID >(
                specimen,
                lifetime,
                massBurn, massDie,
                immunity,
                chemicalComposition, metabolism, survivor
            );

            std::cout << std::endl << "*Муравей*" << std::endl << specimen << std::endl;

            
            // определяем, в каких ячейках области планеты будут жить муравьи
            const auto fnLiving = [] (
                size_t pulse,
                const typelib::coord_t& c,
                const topology_t::living_t::specimen_t& sp
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
                specimen
            );

        } // муравей


#if 0
// - @todo ...
        // цветок
        // @see Прим. выше к декларации муравья.
        {
            topology_t::living_t::specimen_t  specimen;

            const size_t lifetime = 5.0;

            const double massBurn = 0.1 / 1000.0;
            const double massDie = 500.0 / 1000.0;
            const std::vector< double >  mass = ba::list_of
                ( massBurn )
                ( massDie )
            ;

            const std::vector< double >  chemicalComposition = ba::list_of
                // вода
                ( 0.1 )
                // белки
                ( 0.2 )
                // жиры
                ( 0.5 )
                // углеводы
                ( 1.0 - 0.1 - 0.2 - 0.5 )
            ;


            topology_t::living_t::specimen_t::metabolism_t  metabolism[ topology_t::LIFE_CYCLE ];

            const double oxygenNeed = 1.0 / 1000.0 * DAY_IN_YEAR;
            const double waterNeed = 5.0 / 1000.0 * DAY_IN_YEAR;
            const double carbonAcidNeed = oxygenNeed * 2.0;
            const std::vector< double > chemicalNeed = ba::list_of
                // кислород
                ( oxygenNeed )
                // вода
                ( waterNeed )
                // углекислый газ
                ( carbonAcidNeed )
            ;

            const double oxygenWaste = oxygenNeed * 2.0;
            const double carbonAcidWaste = carbonAcidNeed / 2.0;
            const double honeydewWaste = 10.0 / 1000.0 * DAY_IN_YEAR;
            const std::vector< double > chemicalWaste = ba::list_of
                // кислород
                ( oxygenWaste )
                // углекислый газ
                ( carbonAcidWaste )
                // падь (для питания насекомых)
                @todo UID + скорость?
                ( honeydewWaste )
            ;

            // нормы для человека (100 кг):
            //   - требуемая энергия: не требуется (всё может быть получено из пищи)
            //   - излучаемая энергия: нет (полагаем, что вся полученная
            //     энергия расходуется на поддержание жизни)
            const std::vector< double > energyNeed = ba::list_of
                // тепло
                ( 0 )
                // свет
                ( 0 )
            ;
            const std::vector< double > energyWaste = ba::list_of
                // тепло
                ( 0 )
                // свет
                ( 0 )
            ;

            co::metabolism< GRID, GRID, GRID >(
                metabolism,
                chemicalNeed, chemicalWaste,
                energyNeed, energyWaste,
                lifetime,
                massBurn, massDie
            );


            // условия выживания
            topology_t::living_t::specimen_t::survivor_t  survivor;

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
            co::specimen< GRID, GRID, GRID >(
                specimen,
                lifetime,
                massBurn, massDie,
                immunity,
                chemicalComposition, metabolism, survivor
            );

            
            // определяем, в каких ячейках области планеты будут жить муравьи
            const auto fnLiving = [] (
                size_t pulse,
                const typelib::coord_t& c,
                const topology_t::living_t::specimen_t& sp
            ) -> double {
                // @todo Инициализировать в зависимости от среды обитания 'sp'.
                // добавляем только взрослых особей
                // (для муравьёв это 1-й пульс)
                const double count = (pulse == 1) ? 1000 : 0;
                return count;
            };

            co::living< GRID, GRID, GRID >(
                living,
                static_cast< size_t >( UID_LIVING::UIDL_ANT ),
                // кол-во здоровых и больных муравьёв в ячейке области планеты
                fnLiving,
                // информация об особи
                specimen
            );

        } // цветок
#endif

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
                "Температура превышает возможные физические значения." );
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




    // Информация о структурах
    std::cout << std::endl << planet.topology();



    // III. Покажем результат.
    portulan::io::VolumeVTKVisual::option_t o;
    o[ "size-window" ] = 700;
    o[ "size-point" ] = 1;
    o[ "show-corner" ] = false;
    o[ "show-axes" ] = false;
    const auto color = typelib::json::Variant( "{ 'E': 'FFFFFFFF',  'C': '0000FFFF' }" );
    o[ "rgba" ] = color;
#if 0
    o[ "only" ] = ".temperature";
#endif

    portulan::io::VolumeVTKVisual visual( o );

    
    std::cout << std::endl << "Нажимаем 'q' для изменений планеты..." << std::endl << std::endl;

    const size_t PULSE = 1;
    size_t age = 0;
    while ( true ) {
        std::cout << "Возраст " << age << std::endl;

        //visual << planet;

        std::cout << std::endl;

        visual.wait();

        // одинаково работают оба варианта
#if 1
        //chemistry << PULSE;
#else
        //chemistry( PULSE );
#endif

        age += PULSE;

    } // while




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / process-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
