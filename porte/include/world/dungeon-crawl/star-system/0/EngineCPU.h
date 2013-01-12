#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "ListenerStarSystem.h"
#include <boost/algorithm/minmax_element.hpp>

// # Все комментарии см. в planet::l0::Engine.


#undef CL_NONE


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;


/**
* Движок для моделирования звёздной системы.
*
* # События движка обрабатываются самим движком с помощью
*   методов по умолчанию из класса Listener.
*/
class EngineCPU :
    public EngineWithoutBooster< pns::Portulan, pns::real_t >,
    public ListenerStarSystem< EngineCPU >
{
public:
    typedef std::shared_ptr< EngineCPU >  Ptr;
    typedef std::unique_ptr< EngineCPU >  UPtr;



public:
    EngineCPU( real_t timestep );


    virtual ~EngineCPU();




    /**
    * @return За какое минимальное время 2 тела в системе сблизятся при
    *         текущих скоростях и без воздействия сил, с.
    *
    * #! Метод обновляет статистику.
    *
    * @see calcOptimalTimestep()
    */
    real_t timeConverge();




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );
    void pulse();




private:
    /**
    * Воздействия на элементы звёздной системы.
    * @todo Действовать может не только гравитация других тел.
    */
    void asteroidImpactIn( pns::aboutAsteroid_t*, size_t currentI );
    void planetImpactIn( pns::aboutPlanet_t*,     size_t currentI );
    void starImpactIn( pns::aboutStar_t*,         size_t currentI );




    /**
    * Распределяет запомненные наблюдателем события между участниками
    * (элементами звёздной системы).
    *
    * @see observer_t
    */
    void dealEvent();

    void dealEventCollision( pns::eventTwo_t* );


    void dealEventCollision( pns::eventTwo_t*,  pns::aboutAsteroid_t*,  pns::aboutAsteroid_t* );

    void dealEventCollision(
        pns::aboutAsteroid_t*  a,
        const typelib::VectorT< pns::real_t >&  rva,  
        pns::real_t  rval,
        pns::real_t  deltaTemperatureAAfter,

        const pns::aboutAsteroid_t&  b,
        const typelib::VectorT< pns::real_t >&  rvb,
        pns::real_t  rvbl,
        pns::real_t  deltaTemperatureBAfter,
        const pns::pointerElement_t&  pb
    );


    void dealEventCollision( pns::eventTwo_t*,  pns::aboutAsteroid_t*,  pns::aboutStar_t* );

    void dealEventCollision( pns::eventTwo_t*,  pns::aboutStar_t*,  pns::aboutAsteroid_t* );




    /**
    * В памяти элементов звёздной системы остаются только уникальные события.
    * Память наблюдателя не затрагивается.
    *
    *  #! Порядок запомненных событий не сохраняется.
    *
    * @see observer_t
    */
    void uniqueEvent();




    /**
    * Какая сила действует на тело A со стороны B -> в 'force'.
    *
    * @return false Если тела столкнулись (определяется параметром
    *         'noForceDistance' - сила гравитации не учитывается).
    *         true  Если тело A и B находятся на дистанции, превышающей
    *         'noForceDistance'.
    *
    * @param noForceDistance Расстояние, на котором силы будут отключены. Т.о.
    *        избегаем резкого скачка скоростей.
    */
    static bool forceGravityBodyAImpactIn(
        real_t force[ 3 ],
        const pns::aboutBody_t& a,
        const pns::aboutBody_t& b,
        real_t noForceDistance
    );




    /**
    * @return true Если тела столкнулись (определяется параметром 'distance').
    */
    static bool collision(
        const real_t coordA[ 3 ],
        const real_t coordB[ 3 ],
        real_t collisionDistance
    );




    /**
    * Просматривает события движка, информирует слушателей.
    * notifyAndCompleteEvent() с параметрами вызывается для конкретного элемента.
    */
    void notifyAndCompleteEvent();
    void notifyAndCompleteEvent( pns::aboutAsteroid_t*,  size_t currentI,  pns::deltaElement_t& );
    void notifyAndCompleteEvent( pns::aboutPlanet_t*,    size_t currentI,  pns::deltaElement_t& );
    void notifyAndCompleteEvent( pns::aboutStar_t*,      size_t currentI,  pns::deltaElement_t& );




    /**
    * Собирает статистику для элементов портулана.
    */
    void statistics();




    /**
    * Рассчитывает оптимальный шаг времени, опираясь на результат
    * метода timeConverge().
    */
    void calcOptimalTimestep( int step );




    /**
    * @return Ожидаемое время сближения двух точек.
    *         Отрицательное время означает, что точки удаляются
    *         друг от друга.
    *         0.0 - что точки уже вместе.
    *         Бесконечность - что обе точки никогда не сблизятся.
    */
    static real_t estimateTimeApproaching(
        const pns::point_t& a,
        const pns::point_t& b
    );




    /**
    * Собирает статистику звёздной системы.
    */
    void harvestStatisticsForOptimizeTimestep();




private:
    /**
    * Структура для оптимизации расчётов.
    * Заполняется в процессе работы движка над миром.
    */
    pns::statistics_t mStatistics;
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineCPU.inl"
