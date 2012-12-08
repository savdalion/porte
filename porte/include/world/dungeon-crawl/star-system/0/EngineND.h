#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include <Newton.h>
#include <dgMatrix.h>
#include <dgQuaternion.h>
#include <dgVector.h>


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
class EngineND :
    public EngineWithoutBooster< pd::Portulan >
{
public:
    typedef std::shared_ptr< EngineND >  Ptr;
    typedef std::unique_ptr< EngineND >  UPtr;



public:
    EngineND( portulan_t*, pd::real_t extent, pd::real_t timestep );

    virtual ~EngineND();


    /**
    * @virtual Engine
    */
    virtual void sync();




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );
    void pulse();



private:
    /**
    * Создаёт физ. тело в мире Newton Dynamics.
    */
    void incarnateBody( pd::aboutBody_t* b );


    /**
    * Инициирует матрицу.
    */
    static void init(
        pd::real_t tm[ 16 ],
        const pd::real_t coord[ 3 ],
        const pd::real_t rotation[ 3 ]
    );



    static void applyForceAndTorque(
        const NewtonBody* b,
        pd::real_t timestep,
        int threadIndex
    );



    static void setTransform(
        const NewtonBody* b,
        const pd::real_t* matrix,
        int threadIndex
    );



    static void autoDestruction( const NewtonBody* b );





private:
    pd::real_t mTimestep;

    /**
    * Протяжённость мира.
    */
    const pd::real_t mExtent;

    /**
    * Физический мир (внешний движок).
    */
    NewtonWorld* mPhysicsWorld;

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineND.inl"
