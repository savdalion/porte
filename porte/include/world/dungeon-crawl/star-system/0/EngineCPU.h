#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"

// # ��� ����������� ��. � planet::l0::Engine.


#undef CL_NONE


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pd = portulan::world::dungeoncrawl::starsystem::l0;


/**
* ������ ��� ������������� ������� �������.
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
    * ����������� �� �������.
    * @todo �������������� ����� �� ������ ���������� ������ ���.
    */
    void planetInteraction( pd::aboutPlanet_t*, size_t currentI );


    /**
    * ����������� �� ������.
    * @todo �������������� ����� �� ������ ���������� ������ ���.
    */
    void starInteraction( pd::aboutStar_t*, size_t currentI );


    /**
    * �������� ���� B �� ���� A: ��� �������� ���� �� ���� A.
    *
    * @param noForceDistance ����������, �� ������� ���� ����� ���������. �.�.
    *        �������� ������� ������ ���������.
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
