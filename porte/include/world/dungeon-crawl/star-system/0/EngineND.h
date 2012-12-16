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


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


/**
* Движок для моделирования звёздной системы.
*/
class EngineND :
    public EngineWithoutBooster< pns::Portulan >
{
public:
    typedef std::shared_ptr< EngineND >  Ptr;
    typedef std::unique_ptr< EngineND >  UPtr;



public:
    EngineND( portulan_t*, pns::real_t extent, pns::real_t timestep );

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
    void incarnateBody( pns::aboutBody_t* b );


    /**
    * Инициирует матрицу.
    */
    static void init(
        pns::real_t tm[ 16 ],
        const pns::real_t coord[ 3 ],
        const pns::real_t rotation[ 3 ]
    );



    static void applyForceAndTorque(
        const NewtonBody* b,
        pns::real_t timestep,
        int threadIndex
    );



    static void setTransform(
        const NewtonBody* b,
        const pns::real_t* matrix,
        int threadIndex
    );



    static void autoDestruction( const NewtonBody* b );





private:
    pns::real_t mTimestep;

    /**
    * Протяжённость мира.
    */
    const pns::real_t mExtent;

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
