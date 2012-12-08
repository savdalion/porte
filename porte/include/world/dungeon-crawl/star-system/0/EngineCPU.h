#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"

// # Все комментарии см. в planet::l0::Engine.


#undef CL_NONE


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pd = portulan::world::dungeoncrawl::starsystem::l0;


/**
* Движок для моделирования звёздной системы.
*/
class EngineCPU :
    public EngineWithoutBooster< pd::Portulan >
{
public:
    typedef std::shared_ptr< EngineCPU >  Ptr;
    typedef std::unique_ptr< EngineCPU >  UPtr;



public:
    EngineCPU( portulan_t*, pd::real_t timestep );

    virtual ~EngineCPU();




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );
    void pulse();




private:
    /**
    * Воздействия на планету.
    * @todo Воздействовать может не только гравитация других тел.
    */
    void planetInteraction( pd::aboutPlanet_t*, size_t currentI );


    /**
    * Воздействия на звезду.
    * @todo Воздействовать может не только гравитация других тел.
    */
    void starInteraction( pd::aboutStar_t*, size_t currentI );


    /**
    * Действие тела B на тело A: как меняется сила на тело A.
    *
    * @param noForceDistance Расстояние, на котором силы будут отключены. Т.о.
    *        избегаем резкого скачка скоростей.
    */
    static typelib::vectorDouble_t forceGravityBodyBodyInteraction(
        const typelib::coordDouble_t&  coordA,
        const typelib::coordDouble_t&  coordB,
        pd::real_t massA,
        pd::real_t massB,
        pd::real_t noForceDistance
    );




private:
    pd::real_t mTimestep;
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineCPU.inl"
