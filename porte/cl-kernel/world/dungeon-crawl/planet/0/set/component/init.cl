// @require Библиотеки из "include".


/**
* Инициализирует компоненты в области планеты.
*   # Компоненты помещаются на планету равномерно, сохраняя заданные в
*     aboutPlanet_t пропорции.
*/




/**
* Инициализирует компоненты в заданной зоне области планеты.
*   # Проверка, что координата находится в требуемой зоне уже осуществлена.
*   # Пропорции компонентов при инициализации не зависят от глубины залегания.
*/
inline void zoneComponent( __global componentCell_t c, __global const zoneComponent_t* z ) {
    /* @test
    for (uint k = 0; k < COMPONENT_CELL; ++k) {
        c[k].code  = CC_NONE;
        c[k].count = 0.0f;
    }
    return;
    */


    // # В 1-й ячейке храниться по COMPONENT_CELL компонентов.
    uint k = 0;
    for ( ; k < COMPONENT_CELL; ++k) {
        // c[k] typeof portionComponent_t
        // # Может быть передано не более COMPONENT_CELL компонентов.
        const enum CODE_COMPONENT code = z[k].code;
        /* - @todo opimize Можно ускорить. Но наличие этой проверки
             не всё ? инициализирует.
        if (code == CC_NONE) {
            // пустой компонент; значит, дальше ничего нет
            break;
        }
        */
        
        const float count = z[k].count;
        
        // отсеиваем мусор: значение доли компонентов должно лежать
        // в диапазоне [0.0; 1.0]
        // @todo fine Вероятно, ошибка при начальной инициализации структур. Сообщать явно.
        const bool outOfBorder =
            (code <= CC_NONE) || (code >= CC_last) ||
            (count < 0.0f)    || (count > 1.0f);
        //(*c)[k].code  = select( code,  CC_NONE, outOfBorder );
        //(*c)[k].count = select( count, 0.0f,    outOfBorder );
        if ( outOfBorder ) {
            c[k].code  = CC_NONE;
            c[k].count = 0.0f;
        } else {
            c[k].code  = code;
            c[k].count = count;
        }
    }

    // дозаполняем пустотой
    for ( ; k < COMPONENT_CELL; ++k) {
        c[k].code  = CC_NONE;
        c[k].count = 0.0f;
    }
}




inline void coreComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    /* - Свёрнуто. См. ниже.
    // # В 1-й ячейке храниться по COMPONENT_CELL компонентов.
    uint k = 0;
    for ( ; k < COMPONENT_CELL; ++k) {
        // c[n] typeof portionComponent_t
        // # Для планеты может быть передано не более COMPONENT_CELL компонентов.
        const enum CODE_COMPONENT code = ap->component.core[k].code;
        if (code == CC_NONE) {
            // пустой компонент, значит дальше ничего нет, заканчиваем
            break;
        }
        c[k]->code  = code;
        c[k]->count = ap->component.core[k].count;
    }
    // дозаполняем пустотой
    for ( ; k < COMPONENT_CELL; ++k) {
        c[k]->code  = CC_NONE;
        c[k]->count = 0.0f;
    }
    */
    zoneComponent( c, ap->component.core );
}




inline void mantleComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.mantle );
}




inline void crustComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.crust );
}




inline void atmosphereComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.atmosphere );
}




inline void spaceComponent( __global componentCell_t c, __global const aboutPlanet_t* ap ) {
    zoneComponent( c, ap->component.space );
}






__kernel void init(
    __global const aboutPlanet_t*  ap,    // 0
    __global componentCell_t*      cc     // 1
) {
    // всегда - координаты ячейки 3D-карты
    const uint dnx = get_global_id( 0 );
    const uint dny = get_global_id( 1 );
    const uint dnz = get_global_id( 2 );
    const uint4 dnc = (uint4)( dnx, dny, dnz, 0 );
    const int4 nc = normalizeCoord( dnc );

    // @see Опцию -D при при построении ядра  
    const uint i = icDenorm( dnc );

    
    /* test
    uint k = 0;
    for ( ; k < COMPONENT_CELL / 2; ++k) {
        cc[i][k].code  = CC_AIR;
        cc[i][k].count = 1.0f;
    }
    
    for ( ; k < COMPONENT_CELL; ++k) {
        cc[i][k].code  = CC_NONE;
        cc[i][k].count = 0.0f;
    }
    
    return;
    */
    
    
    
    // Компоненты разделены по зонам области планеты.
    // Работаем с реальными размерами.
    
    const float distance = distanceNC( nc ) * (float)SCALE;
    const float halfSize = ap->size / 2.0f;
    const float distanceByHalfSize = distance / halfSize;
    
    // заполняем зоны
    if ( coreZone( ap, distanceByHalfSize ) ) {
        coreComponent( cc[i], ap );
        
    } else if ( mantleZone( ap, distanceByHalfSize ) ) {
        mantleComponent( cc[i], ap );

    } else if ( crustZone( ap, distanceByHalfSize ) ) {
        crustComponent( cc[i], ap );
        
    } else if ( atmosphereZone( ap, distanceByHalfSize ) ) {
        atmosphereComponent( cc[i], ap );
        
    } else {
        spaceComponent( cc[i], ap );
    }
    
}
