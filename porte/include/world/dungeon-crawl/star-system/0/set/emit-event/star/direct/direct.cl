#include "pragma.hcl"
#include "restruct.hcl"
#include "helper.hcl"


/**
* Прямые события от элементов звёздной системы.
*
* @see Коммент. в starsystem::l0::EngineHybrid::pulse().
* @see Ядро relative().
*
* #i NVIDIA 8800GTS работает только с float.
*/




// @see world/dungeon-crawl/star-system/0/set/structure.h
__kernel void direct(
    __global const todayStar_t*   ts,       // 0
    __global emitterEventStar_t*  ees,      // 1
    const real_t                  timestep  // 4
) {
    //  индекс элемента
    const uint i = get_global_id( 0 );
    // @todo optimize Можно не проверять.
    if (i >= STAR_COUNT) {
        return;
    }

    __global characteristicStar_t* element = &ts[ i ];
    if ( absentStar( element ) ) {
        return;
    }

    Индекс излучаемого сейчас события?
    Или каждое событие будет знать, в какую ячейку памяти сохраниться?
    
    //  индекс содержимого памяти событий элемента
    const uint im = get_global_id( 1 );
    // @todo optimize Можно не проверять.
    if (im >= EMIT_EVENT_STAR_COUNT) {
        return;
    }


    __global eventTwo_t* e = &ee[ i ][ im ];



    __global aboutStar_t* element = &as[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
    __global eventTwo_t* eec = element->emitterEvent.content;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        //printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
        //    element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


    // Обязательные события

    // Гравитация
    if (w < EMITTER_EVENT_COUNT) {
        // вычислим часть формулы для расчёта силы притяжения: это значение
        // смогут использовать элементы, которые притягивает звезда
        const real_t fgm = G * element->today.mass;
        const eventTwo_t e = {
            // uid события
            E_GRAVITY,
            // второй участник события - здесь не важен
            {},
            { fgm }
        };
        //eec[ w ] = e;
        eec[ w ].uid = E_GRAVITY;
        eec[ w ].fReal[ 0 ] = fgm;
        ++w;
    }


    // Излучение энергии (ядерная реакция)
    // # Каждую секунду звезда излучает энергию.
    // @todo optimize Запоминать вычисленные значения.
    const real_t radiusA = element->today.radius;
    const real_t surfaceTemperatureA = element->today.surfaceTemperature;
    const real_t luminosityABySecond =
        luminosity( radiusA, surfaceTemperatureA );
    if (w < EMITTER_EVENT_COUNT) {
        const eventTwo_t e = {
            // uid события
            E_RADIATION,
            // второй участник события - здесь не важен
            {},
            { luminosityABySecond, luminosityABySecond * timestep }
        };
        eec[ w ] = e;
        //eec[ w ].uid = E_RADIATION;
        //eec[ w ].fReal[ 0 ] = luminosityABySecond;
        //eec[ w ].fReal[ 1 ] = luminosityABySecond * timestep;
        ++w;
    }


    // Потеря массы (из-за излучения)
    // @todo optimize Запоминать вычисленные значения.
    // E = m * c ^ 2
    const real_t deltaMassABySecond =
        -luminosityABySecond / (SPEED_LIGHT * SPEED_LIGHT);
    if (w < EMITTER_EVENT_COUNT) {
        const eventTwo_t e = {
            // uid события
            E_CHANGE_MASS,
            // второй участник события - здесь не важен
            {},
            { deltaMassABySecond, deltaMassABySecond * timestep }
        };
        eec[ w ] = e;
        ++w;
    }


    // Проверка на столкновения
    // # Проверка на столкновения осуществляется идущими выше по алфавиту
    //   элементами. См. соглашение в теле EngineHybrid::pulse().


    // обновляем вальдо
    ee->waldo = w;

}
