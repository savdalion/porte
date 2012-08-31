#include "../include/StdAfx.h"
#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>



/**
* Визуальное тестирование для проекта 'Porte'.
* Химические превращения веществ (материй).
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace co = portulan::command::common;


    // Приоритет определяет размер лог-файла
    axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_rarely );
    //axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_often );

    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / chemistry-visualtest -> BEGIN" << std::endl;
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
    typedef portulan::Portulan< GRID, GRID, GRID >  plato_t;
    plato_t plato;


    // I. Соберём карту из карты высот.

    // холм из каменной соли
    const std::string elevationFile = PATH_MEDIA + "a/center-plato-elevation-map.png";
    //const std::string elevationFile = PATH_MEDIA + "a/minimax-elevation-map.png";
    // размеры задаются в км
    // картинка "a/center-hill-elevation-map.png" протяжённостью
    // = 100 м (размер изображения = 400 пкс)
    // квадратная картинка
    const size_t sizeImage = 400;
    const double scaleXY = 0.100 / static_cast< double>( sizeImage );
    // высота холма = 50 м
    const double hMin = -0.010;
    const double hMax =  0.040;
    const double h = hMax - hMin;
    /* - Работаем проще. См. ниже.
    city << co::elevationMap< GRID, GRID, GRID, float >( "NaCl", elevationFile, scaleXY, hMin, hMax );
    */
    co::elevationMap( plato, "NaCl", elevationFile, scaleXY, hMin, hMax );


    // заливаем водой места, которые задаёт маска
    const std::string waterFile = PATH_MEDIA + "a/water1-mask-map.png";
    // указываем глубину в ячейках
    //const int gridHMin = plato_t::topology_t::signBitLayer_t::minCoord().z;
    const int gridHMin = -15;
    const int gridHMax = 0;
    co::flood( plato, "H2O", waterFile, gridHMin, gridHMax );



    // II. Инициируем химический движок.
    typedef Chemistry< GRID, GRID, GRID >  chemistry_t;
    chemistry_t chemistry( &plato );

    // описываем хим. реакцию
    // NaCl(соль) + H2O(вода) =
    //   = Na+(катион натрия, натрий без электрона) + Cl-(анион хлора, хлор с электроном натрия) + H2O(вода)

#if 0
// - Рассуждения. Оставлены для справки. Решение - см. ниже.

    /* - Нет. Реализуем через ссылки на функции. См. ниже.
    class DissolutionSalt: public ChemicalReaction {
        inline ...
    };
    chemistry << DissolutionSalt();
    */

    typedef ChemicalReaction< GRID, GRID, GRID >  chemicalReaction_t;
    chemicalReaction_t dissolutionSalt( plato );

    // условия для начала реакции
    /* - Достаточно скорости протекания? См. ниже.
    dissolutionSalt.need = boost::assign::list_of
        ("NaCl")
        ("H2O")
    ;
    */

    (?) Реализуя реакции функциями (см. ниже) получаем гибкость определения,
        но лишаемся возможности передать выполнение параллельным алгоритмам.
        Реакции ведь можно чётко формализовать...

    (!) Растворы - это отдельные вещества, их свойства отличаются от
        свойств реагентов.

    (#) Ионы перемещаются по ячейкам объёма вместе с растворителем.

    // скорость обмена (взаимное проникновение, смешивание) элементов между
    // соседними ячейками (есть хим. реакции, которые проходят внутри ячеек)
    // (!) но соль не только смешивается с водой, но и растворяется; а когда
    // раствор перенасыщен - соль выпадает в осадок...
    const auto mixing = [] ( const plato_t& env, const typelib::coordInt_t& ca, const typelib::coordInt_t& cb ) -> float {
        // для начала реакции смешивания достаточно, чтобы соль и вода
        // находились в соседних ячейках
        const bool need = ChemicalReaction::need( env, ca, cb, "NaCl", "H2O" );
        if ( !need ) {
            return 0.0f;
        }
        // некоторое кол-во (%) NaCl и H2O смешиваются за 1 пульс
        return 0.01f;
    };

    // образуется раствор, не новый элемент
    /* - Поэтому можем не определять out-методы.
    typedef std::pair< typelib::Sign<>, float >  signPercent_t;
    dissolutionSalt.out1 = [] () -> signPercent_t {
        return signPercent_t( ... );
    };
    */

    // теплота не выделяется и не поглощается
    /* - Поэтому можем не определять heat-метод.
    dissolutionSalt.heat = [] () -> float {
        return 0.0f;
    };
    */

    chemistry << dissolutionSalt;

#endif


#if 0
// - @todo ...
    // хим. реакцию оформим картой данных: т.о. можно распараллелить

    /* - Другой вариант. См. ниже.
    // условие начала реакции
    // # последним идёт растворитель
    reactionNaClH2O.in() << "NaCl" << "H2O";

    // продукты реакции
    reactionNaClH2O.out() << "Na+" << "Cl-" << "H2O";

    // какое кол-во (%) NaCl растворяется в H2O за 1 пульс
    // (как меняется концентрация веществ с течением времени)
    // агрег. состояния растворителя (H2O):
    // [ твёрдое тело, жидкость, газ, плазма ]
    // # Скорость образования продуктов реакции одинакова для всех продуктов.
    const float speed[] = { 0.0f, 0.01f, 0.02f, 0.03f };
    reactionNaClH2O.speed( speed );

    // при каком кол-ве (%) вещества получаем насыщенный раствор
    // (в 100 г H2O можно растворить 36 г NaCl при 20 C и 40 г при 100 C)
    // # Насыщенность задаётся для всего, что не растворитель.
    const float satsol[] = { 0.0f, 0.36f, 0.40f, 0.50f };
    reactionNaClH2O.satsol( satsol );
    */

    // # Условия, при которых реакция не проходит, не записываются.
    //   Например, соль не будет растворяться в замёрзшей воде.

    // (1) Реакция при температуре от 0 до 100 C
    {
        ChemicalReaction reaction;

        // условие начала реакции
        // # последним идёт растворитель
        //reactionNaClH2O.in() << "NaCl" << "H2O" << "0 < .temperature < 99";
        reaction << "NaCl" << "H2O";
        reaction.temperature( 0.0f, 99.0f );

        // продукты реакции
        reaction >> "Na+" >> "Cl-" >> "H2O";

        // какое кол-во (%) NaCl растворяется в H2O за 1 пульс
        // (как меняется концентрация веществ с течением времени)
        // # Скорость образования продуктов реакции одинакова для всех продуктов.
        reaction.speed( 0.01f );

        // при каком кол-ве (%) вещества получаем насыщенный раствор
        // (в 100 г H2O можно растворить 36 г NaCl при 20 C и 40 г при 100 C)
        // # Насыщенность задаётся для всего, что не растворитель.
        reaction.satsol( 0.36f );

        // какое кол-во теплоты выделяется / поглощается при реакции
        reaction.heat( 0.0f );

        // обучаем хим. движок реакции
        chemistry << reaction;
    }


    // (2) Реакция при температуре ниже 0 C
    {
        ChemicalReaction reaction;

        reaction << "NaCl" << "H2O";
        reaction.temperature( -std::numeric_limits< float >::infinity(),  0.0f );
        /* - ничего нового не образуется, нет реакции
        reaction.out() << "Na+" << "Cl-" << "H2O";
        */
        reaction.speed ( 0.0f );
        reaction.satsol( 0.0f );
        reaction.heat  ( 0.0f );

        chemistry << reaction;
    }


    // (2) Реакция при температуре выше 99 C
    {
        ChemicalReaction reaction;

        reaction << "NaCl" << "H2O";
        reaction.temperature( 99.0f,  std::numeric_limits< float >::infinity() );
        reaction >> "Na+" >> "Cl-" >> "H2O";
        reaction.speed ( 0.02f );
        reaction.satsol( 0.40f );
        reaction.heat  ( 0.0f  );

        chemistry << reaction;
    }

#endif






    // III. Покажем результат.
    portulan::io::VolumeVTKVisual::option_t o;
    o[ "size-window" ] = 700;
    o[ "size-point" ] = 1;
    o[ "show-corner" ] = false;
    o[ "show-axes" ] = false;
    const auto color = typelib::json::Variant( "{ 'NaCl': 'FFFFFFFF',  'H2O': '0000FFFF' }" );
    o[ "rgba" ] = color;
#if 0
    o[ "only" ] = ".temperature";
#endif

    portulan::io::VolumeVTKVisual visual( o );

    
    std::cout << std::endl << "Нажимаем 'q' для просмотра растворения в объёме..." << std::endl << std::endl;

    const size_t PULSE = 1;
    size_t age = 0;
    while ( true ) {
        std::cout << "Возраст " << age << std::endl;

        visual << plato;

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




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / chemistry-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
