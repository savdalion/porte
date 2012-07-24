#include "../include/StdAfx.h"
#include "../include/configure.h"
#include <porte/porte.h>
#include <portulan/portulan.h>



/**
* Визуальное тестирование для проекта 'Porte'.
* Распространение тепла (температуры) в заданном объёме для *однородной* среды.
*/
int main( int argc, char** argv ) {

    using namespace porte;
    using namespace porte::visualtest;

    namespace co = portulan::command;


    // Приоритет определяет размер лог-файла
    axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_rarely );
    //axter::ezlogger<>::set_verbosity_level_tolerance( axter::log_often );

    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / uht-visualtest -> BEGIN" << std::endl;
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
    typedef portulan::Portulan< GRID, GRID, GRID >  city_t;
    city_t city;


    // I. Соберём карту температур.
    
    const co::fnTemperature_t& fnt = [] ( const typelib::coordInt_t& c ) -> float {
        // @test Постоянная температура во всех ячейках
        //return 20.0f;

        // @test Температура зависит от высоты Z
        //return -static_cast< float >( c.z ) * 10.0f;

        // @test Температура зависит от близости к центру
        return 4800.0f - static_cast< float >( c.x * c.x + c.y * c.y + c.z * c.z );
    };

    co::temperature( city, fnt );


    // II. Инициируем движок теплообмена для однородной среды.
    /* - Не так. Отдаём портулан движку при инициализации. См. ниже.
    // Для быстрой обработки портулана, обернём его в спец. класс Booster
    PortulanBooster< GRID, GRID, GRID >  cityBooster( &city );
    */

    typedef UniformHeatTransfer< GRID, GRID, GRID >  uniformHeatTransfer_t;
    uniformHeatTransfer_t uniformHeatTransfer( &city );

    
    // III. Покажем результат.
    portulan::io::VolumeVTKVisual::option_t o;
    o[ "size-window" ] = 700;
    o[ "size-point" ] = 1;
    o[ "show-corner" ] = false;
    o[ "show-axes" ] = false;
    o[ "only" ] = ".temperature";
#if 0
    // включаем, если хотим видеть температуры в фиксированном диапазоне цветов
    o[ "color-range" ] = typelib::json::Variant( "[ 0, 4800 ]" );
#endif

    portulan::io::VolumeVTKVisual visual( o );

    
    std::cout << std::endl << "Нажимаем 'q' для изменения температуры в объёме..." << std::endl << std::endl;

    const size_t PULSE = 1000;
    size_t age = 0;
    while ( true ) {
        std::cout << "Возраст " << age << std::endl;
        std::cout << "Общая температура: " << city.topology().temperature().sum() << std::endl;

        visual << city;

        std::cout << std::endl;

        visual.wait();

        /* - Заменено. См. ниже.
        // одинаково работают оба варианта: первый - проще, второй - гибче
#if 1
        uniformHeatTransfer >> cityBooster;
#else
        uniformHeatTransfer( cityBooster, 1 );
#endif
        */

        // одинаково работают оба варианта
#if 1
        uniformHeatTransfer << PULSE;
#else
        uniformHeatTransfer( PULSE );
#endif

        age += PULSE;

    } // while




    EZLOGGERVLSTREAM( axter::log_always ) << "Porte / uht-visualtest -> END" << std::endl;

    std::cout << std::endl << "^" << std::endl;
    //std::cin.ignore();

    return 0;

} // main()
