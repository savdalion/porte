// @require Библиотеки из "include".


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
    __global const aboutStarSystem_t*  ass,      // 0
    __global aboutAsteroid_t*          aa,       // 1
    __global aboutPlanet_t*            ap,       // 2
    __global aboutStar_t*              as,       // 3
    const real_t                       timestep  // 4
) {
    // # Сюда получаем готовый индекс. Учитываем, что кол-во элементов
    //   в группах - разное.
    const uint i = get_global_id( 0 );

    if ( (i > STAR_COUNT) || absentStar( &as[ i ] ) ) {
        return;
    }


    __global aboutStar_t* element = &as[ i ];
    __global emitterEvent_t* ee = &element->emitterEvent;
#ifdef __DEBUG
    if ( !betweenInteger( ee->waldo, 0, EMITTER_EVENT_COUNT - 1 ) ) {
        printf( "(?) Star %d is not initialized or it memory is overfilled. Waldo = %i.\n",
            element->uid, ee->waldo );
    }
#endif

    int w = ee->waldo;


    // Обязательные события

    // Гравитация
    const real_t massA = massStar( element );
    if (w < EMITTER_EVENT_COUNT) {
        // вычислим часть формулы для расчёта силы притяжения: это значение
        // смогут использовать элементы, которые притягивает звезда
        const real_t fgm = G * massA;
        eventTwo_t e = {
            // uid события
            E_GRAVITY,
            // второй участник события - здесь не важен
            {},
            { fgm }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Излучение энергии (ядерная реакция)
    // # Каждую секунду звезда излучает энергию.
    // @todo optimize Запоминать вычисленные значения.
    const real_t radiusA = element->today.radius;
    const real_t surfaceTemperatureA = element->today.surfaceTemperature;
    const real_t luminosityBySecond =
        luminosity( radiusA, surfaceTemperatureA );
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_RADIATION,
            // второй участник события - здесь не важен
            {},
            { luminosityBySecond, luminosityBySecond * timestep }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Потеря массы (из-за излучения)
    // @todo optimize Запоминать вычисленные значения.
    // E = m * c ^ 2
    const real_t deltaMassBySecond =
        -luminosityBySecond / (SPEED_LIGHT * SPEED_LIGHT);
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_CHANGE_MASS,
            // второй участник события - здесь не важен
            {},
            { deltaMassBySecond, deltaMassBySecond * timestep }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }
    if (w < EMITTER_EVENT_COUNT) {
        eventTwo_t e = {
            // uid события
            E_DECREASE_MASS,
            // второй участник события - здесь не важен
            {},
            { deltaMassBySecond, deltaMassBySecond * timestep }
        };
        element->emitterEvent.content[ w ] = e;
        ++w;
    }


    // Проверка на столкновения
    // # Проверка на столкновения осуществляется идущими выше по алфавиту
    //   элементами. См. соглашение в теле EngineHybrid::pulse().


    // обновляем вальдо
    ee->waldo = w;

}
