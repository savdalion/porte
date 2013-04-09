#include "pragma.hcl"
#include "restruct.hcl"


/**
* Подготавливает элементы звёздной системы к циклу испускания событий.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядра direct(), relative(), fix().
*
* #i NVIDIA 8800GTS работает только с float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void begin(
    __global emitterEventStar_t*  ee  // 0
) {
    //  индекс элемента
    const uint i = get_global_id( 0 );
    // @todo optimize Можно не проверять.
    if (i >= STAR_COUNT) {
        return;
    }

    //  индекс содержимого памяти событий элемента
    const uint im = get_global_id( 1 );
    // @todo optimize Можно не проверять.
    if (im >= EMIT_EVENT_STAR_COUNT) {
        return;
    }


    __global eventTwo_t* e = &ee[ i ][ im ];





    // @test
#if 0
    printf( "aboutStarSystem_t %i\n", sizeof( aboutStarSystem_t ) );
    printf( "aboutAsteroid_t %i\n", sizeof( aboutAsteroid_t ) );
    printf( "aboutPlanet_t %i\n", sizeof( aboutPlanet_t ) );
    printf( "aboutStar_t %i\n", sizeof( aboutStar_t ) );
    printf( "characteristicAsteroid_t %i\n", sizeof( characteristicAsteroid_t ) );
    printf( "real_t %i\n", sizeof( real_t ) );
    printf( "real2_t %i\n", sizeof( real2_t ) );
    printf( "real3_t %i\n", sizeof( real3_t ) );
    printf( "real4_t %i\n", sizeof( real4_t ) );
    printf( "bool %i\n", sizeof( bool ) );
    printf( "eventTwo_t %i\n", sizeof( eventTwo_t ) );
    printf( "pointerElement_t %i\n", sizeof( pointerElement_t ) );
    printf( "enum EVENT %i\n", sizeof( enum EVENT ) );
    printf( "memoryModel_t %i\n", sizeof( memoryModel_t ) );
    printf( "model_t %i\n", sizeof( model_t ) );
    printf( "frequencyMemoryModel_t %i\n", sizeof( frequencyMemoryModel_t ) );
    printf( "frequencyModel_t %i\n", sizeof( frequencyModel_t ) );
    printf( "\n" );
    return;
#endif


    // обнуляем ячейку события
    // @todo optimize Можно сбрасывать только UID события.
    const eventTwo_t EMPTY = {};
    *e = EMPTY;

}
