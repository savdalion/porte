#pragma once

#include "../../../../../configure.h"

#include "EngineHybrid.h"



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
    public EngineHybrid
{
public:
    typedef std::shared_ptr< Engine >  Ptr;
    typedef std::unique_ptr< Engine >  UPtr;



public:
    explicit inline Engine( real_t timestep ) :
        EngineHybrid( timestep )
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
