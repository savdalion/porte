#include "include/pragma.hcl"
#include "include/restruct.hcl"
#include "include/helper.hcl"
#include "include/zone.hcl"
#include "include/dice.hcl"


/**
* Инициализирует особей в области планеты.
*
* #! Перед вызовом этого ядра следует выполнить ядро "clear".
*/



#ifdef WITH_NEIGHBOUR_EXPANSION
/**
* @return Кол-во соседних особей.
*
* @param code Если код = CL_NONE, считается кол-во любых особей.
*/
inline float neighbourCount(
    const enum CODE_LIVING code,
    const enum LIFE_CYCLE lifeCycle,
    __global livingCell_t* lc,
    const int4 nc
) {
    float count = 0.0f;

    // проверяем горизонтальные и вертикальные ячейки,
    // диагонали не трогаем
    const static uint side[6] = { 0, 1, 2, 3, 9, 18 };
    for (uint q = 0; q < 6; ++q) {
        const int4 neighbourNC = isc( side[q] );
        const uint ni = ic( neighbourNC );
        for (uint o = 0; o < LIVING_CELL; ++o) {
            if ( (code == CL_NONE) || (code == lc[ ni ][ o ][ lifeCycle ].code) ) {
                count += lc[ ni ][ o ][ lifeCycle ].count;
            }
        }
    }

    return count;
}
#endif




/**
* Инициализирует особей в заданной зоне области планеты.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*/
inline void zoneLiving(
    __global livingCell_t* lc,
    __global const zoneLiving_t* z,
    __global const zoneLivingCountComplete_t* zcc,
    const enum LIFE_CYCLE lifeCycle,
    uint2* rstate,
    const int4 nc,
    const uint i
) {
    // Кол-во особей в области планеты формируется итерационно.

    // проходим по всем известным в этой зоне планеты особям
    // # Одна ячейка содержит не более LIVING_CELL особей.
    for (uint k = 0; k < LIVING_CELL; ++k) {
        // lc[i][k][LC] typeof portionLiving_t
        // # Может быть передано не более LIVING_CELL особей.
        const enum CODE_LIVING code = z[k].code;
        if (code == CL_NONE) {
            // пустая особь, ничего не добавляем
            continue;
        }
        
        // Контролируем, чтобы общее кол-во особей не превысило желаемое.
        // @todo extend? Вести контроль по зонам?
        const bool countComplete = zcc[code][lifeCycle]->all;
        if ( countComplete ) {
            continue;
        }
        
        // подмешаем к генератору новые значения
        rstate->x ^= (uint)code ^ (uint)lifeCycle ^ k;

        // Факт увеличение кол-ва особей определяется вероятностью.
        // вероятность появления новых групп особей в этой ячейке
        uint diceGroup = dice( rstate, 1, 20 );

        // проверяем, есть ли уже этот вид особи в этой ячейке
        uint o = 0;
        for (; o < LIVING_CELL; ++o) {
            if (lc[i][o][lifeCycle].code == code) {
                // особь уже есть
                // ещё более разнообразим генератор
                rstate->y ^= (uint)( lc[i][o][lifeCycle].count );
                break;
            }
        }
        const bool absent = (o >= LIVING_CELL);


        // Вводим "коэффициент роста": он позволит сделать кол-ва не плоскими.
        //float grow = floatDice( rstate, 1, 20.0 );
        float grow = floatDiapasonRandom( rstate,  (float2)( -0.2f, 1.0f ) );


#ifdef WITH_SELF_EXPANSION
        // Наличие в текущей ячейке подобных особей повышает вероятность
        // увеличения кол-ва особей в этой же ячейке.
        if ( !absent ) {
            diceGroup++;
            diceGroup++;
            grow *= 2.0f;
        }
#endif

#ifdef WITH_NEIGHBOUR_EXPANSION
        // Наличие в соседних ячейках подобных особей повышает вероятность
        // увеличения кол-ва особей в текущей ячейке.
        // смотрим на соседей
        // #! Т.к. расселение особей ведётся без промежуточной матрицы
        //    и в ядре OpenCL отсутствует возможность синхронизовать потоки
        //    на глобальном уровне, возможна неточность при подсчёте кол-ва
        //    соседей. Здесь ошибка в кол-ве - вполне приемлема. Но надо
        //    помнить, что в общем случае она влечёт разное наполнение
        //    матриц даже при одинаковом "seed" (см. __kernel).
        const float n = neighbourCount( code, lifeCycle, lc, nc );
        if (n > 0.0f) {
            diceGroup++;
            grow *= 1.3f;
        }
        if (n > 1000000.0f) {
            diceGroup++;
            grow *= 1.3f;
        }
        if (n > 1000000000.0f) {
            diceGroup++;
            grow *= 1.3f;
        }
#endif

        if (diceGroup < 15) {
            // новые группы этих особей - определ. по "code" - не появляются
            continue;
        }
        
        // добавляем новые группы особей "code"
        if ( absent ) {
            // устанавливаем указатель на первую свободную ячейку
            o = 0;
            for ( ; o < LIVING_CELL; ++o) {
                if (lc[i][o][lifeCycle].code == CL_NONE) {
                    // ячейка с инф. об особи свободна
                    break;
                }
            }            
        }
        if (o >= LIVING_CELL) {
            // все места заняты, ничто новое не может быть добавлено
            // в эту ячейку
            // @todo optimize bad Лишняя проверка. Ведь # кол-во переданных
            //       в параметрах особей никогда не превышает макс. кол-ва
            //       мест для особей в ячейке.
            return;
        }
        // здесь "o" гарантированно указывает на свободное место для особи

        // Т.к. может потребоваться "разместить" сотни млрд особей,
        // ускоряем этот процесс, оценивая базовые кол-ва.
        /* - не используем.
        const float minGroup = z[k].minGroup;
        const float maxGroup = z[k].maxGroup;
        const float averageGroup = (minGroup + maxGroup) / 2.0f;
        */

        /* - Другой подход. См. ниже.
        // вычисляем коэффициент с учётом кол-ва ячеек в области планеты и
        // полагая, что расселение происходит равномерно
        static const float VOLUME_K = (float)(GRID * GRID * GRID / 2);

        const float delta =
            z[k].count / averageGroup * (float)GROW_SPEED / VOLUME_K;

        lc[i][o][lifeCycle].code = code;
        const float qty = (float)intDiapasonRandom(
            rstate,  (int2)( minGroup, maxGroup )
        ) * delta;
        lc[i][o][lifeCycle].count += qty;
        */
        
        lc[i][o][lifeCycle].code = code;
        static const float VOLUME_K = (float)(GRID * GRID * GRID / 9 + 1);
        const float powerIteration = z[k].count / ((float)ITERATION_GROW_COUNT * VOLUME_K);
        const float qty = grow * floatDice( rstate, 1, powerIteration );
        lc[i][o][lifeCycle].count += qty;
        // @todo optimize Целостность можно проверить после всех init-итераций.
        if (lc[i][o][lifeCycle].count < 1.0f) {
            lc[i][o][lifeCycle].count = floatDice( rstate, 1, powerIteration ) + 1.0f;
        }

    } // for (uint k

}





__kernel void init(
    __global const aboutPlanet_t*               ap,    // 0
    __global livingCell_t*                      lc,    // 1
    // # Не используем группировку по зонам для проверки кол-ва.
    __global const zoneLivingCountComplete_t*  zcc,    // 2
    const enum LIFE_CYCLE                lifeCycle,    // 3
    __global const componentCell_t*             cc,    // 4
    __global const temperatureCell_t*           tc,    // 5
    const uint                                seed     // 6
) {
    // всегда - координаты ячейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see Опцию -D при при построении ядра  
    const uint i = icDenorm( dnc );


    // инициализируем генератор случ. чисел для этой ячейки
    uint2 rstate = (uint2)( seed, i );


    // Компоненты разделены по зонам области планеты.
    // Работаем с реальными размерами.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;

    // заполняем зоны
    if ( coreZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.core, zcc, lifeCycle, &rstate, nc, i );

    } else if ( mantleZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.mantle, zcc, lifeCycle, &rstate, nc, i );

    } else if ( crustZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.crust, zcc, lifeCycle, &rstate, nc, i );

    } else if ( atmosphereZone( ap, distanceByHalfSize ) ) {
        zoneLiving( lc, ap->living.atmosphere, zcc, lifeCycle, &rstate, nc, i );

    } else {
        zoneLiving( lc, ap->living.space, zcc, lifeCycle, &rstate, nc, i );
    }

}
