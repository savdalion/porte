#pragma once

#include "../../../../../configure.h"

#if defined( OPENCL_STARSYSTEM_L0_ENGINE_PORTE )
    #include "EngineOpenCL.h"
#elif defined( ND_STARSYSTEM_L0_ENGINE_PORTE )
    #include "EngineND.h"
#else
    #include "EngineCPU.h"
#endif



namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;


/**
* Движок для моделирования *звёздной системы*.
*/
class Engine :
#if defined( OPENCL_STARSYSTEM_L0_ENGINE_PORTE )
    public EngineOpenCL
#elif defined( ND_STARSYSTEM_L0_ENGINE_PORTE )
    public EngineND
#else
    public EngineCPU
#endif
{
public:
    typedef std::shared_ptr< Engine >  Ptr;
    typedef std::unique_ptr< Engine >  UPtr;



public:
    // @todo Добавить 'extent' для всех движков ниже.
    explicit inline Engine( real_t timestep ) :
#if defined( OPENCL_STARSYSTEM_L0_ENGINE_PORTE )
        EngineOpenCL( p, timestep )
#elif defined( ND_STARSYSTEM_L0_ENGINE_PORTE )
        EngineND( p, extent, timestep )
#else
        EngineCPU( timestep )
#endif
    {
    }


    inline virtual ~Engine() {
    }



    /**
    * @virtual 
    */
    virtual real_t extent();



    /**
    * Обновляет состояние элементов согласно значениям, с которыми работает
    * движок. Например, EngineND использует матрицу трансформации для работы
    * с физ. телом. Если мы хотим получить координаты / вращение тела на
    * "сейчас", следует вызвать sync().
    */
    virtual inline void sync() {
        // по умолчанию синхронизации нет
    }

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte







#include "Engine.inl"
