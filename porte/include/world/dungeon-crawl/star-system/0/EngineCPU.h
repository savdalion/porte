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
    public ListenerStarSystem
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
    * notify() с параметрами вызывается для конкретного элемента.
    */
    void notify();
    void notify( pns::aboutAsteroid_t*, size_t currentI, pns::deltaElement_t& );
    void notify( pns::aboutPlanet_t*,   size_t currentI );
    void notify( pns::aboutStar_t*,     size_t currentI );



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
