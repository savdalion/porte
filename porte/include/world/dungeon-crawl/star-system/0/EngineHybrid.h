#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "../../../../porte/CL.h"
#include "../../../../porte/Exception.h"
#include "StoreListenerAsteroid.h"
#include "StoreListenerStar.h"
#include <boost/assign.hpp>

// # Все комментарии см. в planet::l0::Engine.

#include <CL/cl.hpp>


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
class EngineHybrid :
    public EngineWithoutBooster< pns::Portulan, pns::real_t >,
    // # На события этого движка можно подписаться.
    public StoreListenerAsteroid,
    public StoreListenerStar
{
public:
    typedef std::shared_ptr< EngineHybrid >  Ptr;
    typedef std::unique_ptr< EngineHybrid >  UPtr;




public:
    EngineHybrid( real_t timestep );


    virtual ~EngineHybrid();




    /**
    * @virtual EngineWithoutBooster
    */
    virtual void incarnate(
        std::shared_ptr< portulan_t >  p,
        real_t extentPortulan = 0
    );




protected:
    /**
    * @virtual EngineWithoutBooster
    */
    virtual void pulse( int n );




private:
    void emitEvent( int n );




    /**
    * @return Тела столкнулись (определяется параметром 'distance').
    */
    static bool collision(
        const real_t coordA[ 3 ],
        const real_t coordB[ 3 ],
        real_t collisionDistance
    );




    /**
    * Собирает статистику для элементов портулана.
    */
    void statistics();




    void prepareCL();




    void prepare();
    void prepareEmitEvent( const std::string& element );




    void compileCLKernel(
        const std::vector< std::string >&  kernelKeys,
        const std::vector< std::string >&  includeHCL = std::vector< std::string >(),
        const std::string& options = ""
    );




    /**
    * Выполняет ядро OpenCL по имени 'key'.
    *
    * @param waitEvents  Ядро подождёт завершения этих событий.
    * @param event  Если указан, инициируется событием (для синхронизации
    *        потоков).
    */
    typedef VECTOR_CLASS< cl::Event >  vectorEventCL_t;


    template< size_t GLOBAL_SIZE >
    void enqueueEventKernelCL(
        const std::string&  key,
        cl::Event*          event = nullptr
    );


    template< size_t GLOBAL_SIZE >
    void enqueueEventKernelCL(
        const std::string&      key,
        const vectorEventCL_t&  waitEvents,
        cl::Event*              event = nullptr
    );




    static std::string commonConstantCLKernel();
    static std::string commonOptionCLKernel();




private:
    std::map< std::string, cl::Kernel >  mKernelCL;

    const size_t memsizeStarSystem;
    const size_t memsizeAsteroid;
    const size_t memsizePlanet;
    const size_t memsizeStar;

    cl::Buffer aboutStarSystemBCL;
    cl::Buffer asteroidBCL;
    cl::Buffer planetBCL;
    cl::Buffer starBCL;

    cl::Context mContextCL;
    std::vector< cl::Device >  mDeviceCL;
    cl::CommandQueue mQueueCL;

    cl_int errorCL;
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineHybrid.inl"
#include "prepareEngineHybrid.inl"
