#pragma once

#include "../../../../../configure.h"
#include "../../../../porte/EngineWithoutBooster.h"
#include "ListenerStarSystem.h"
#include <boost/algorithm/minmax_element.hpp>

// # ��� ����������� ��. � planet::l0::Engine.


#undef CL_NONE


namespace porte {
    namespace world {
        namespace dungeoncrawl {
            namespace starsystem {
                namespace l0 {


namespace pns = portulan::world::dungeoncrawl::starsystem::l0;
namespace pep = porte::world::dungeoncrawl::planet::l0;


/**
* ������ ��� ������������� ������� �������.
*
* # ������� ������ �������������� ����� ������� � �������
*   ������� �� ��������� �� ������ Listener.
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
    * @return �� ����� ����������� ����� 2 ���� � ������� ��������� ���
    *         ������� ��������� � ��� ����������� ���, �.
    *
    * #! ����� ��������� ����������.
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
    * ����������� �� �������� ������� �������.
    * @todo ����������� ����� �� ������ ���������� ������ ���.
    */
    void asteroidImpactIn( pns::aboutAsteroid_t*, size_t currentI );
    void planetImpactIn( pns::aboutPlanet_t*,     size_t currentI );
    void starImpactIn( pns::aboutStar_t*,         size_t currentI );




    /**
    * ������������ ����������� ������������ ������� ����� �����������
    * (���������� ������� �������).
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
    * � ������ ��������� ������� ������� �������� ������ ���������� �������.
    * ������ ����������� �� �������������.
    *
    *  #! ������� ����������� ������� �� �����������.
    *
    * @see observer_t
    */
    void uniqueEvent();




    /**
    * ����� ���� ��������� �� ���� A �� ������� B -> � 'force'.
    *
    * @return false ���� ���� ����������� (������������ ����������
    *         'noForceDistance' - ���� ���������� �� �����������).
    *         true  ���� ���� A � B ��������� �� ���������, �����������
    *         'noForceDistance'.
    *
    * @param noForceDistance ����������, �� ������� ���� ����� ���������. �.�.
    *        �������� ������� ������ ���������.
    */
    static bool forceGravityBodyAImpactIn(
        real_t force[ 3 ],
        const pns::aboutBody_t& a,
        const pns::aboutBody_t& b,
        real_t noForceDistance
    );




    /**
    * @return true ���� ���� ����������� (������������ ���������� 'distance').
    */
    static bool collision(
        const real_t coordA[ 3 ],
        const real_t coordB[ 3 ],
        real_t collisionDistance
    );




    /**
    * ������������� ������� ������, ����������� ����������.
    * notifyAndCompleteEvent() � ����������� ���������� ��� ����������� ��������.
    */
    void notifyAndCompleteEvent();
    void notifyAndCompleteEvent( pns::aboutAsteroid_t*,  size_t currentI,  pns::deltaElement_t& );
    void notifyAndCompleteEvent( pns::aboutPlanet_t*,    size_t currentI,  pns::deltaElement_t& );
    void notifyAndCompleteEvent( pns::aboutStar_t*,      size_t currentI,  pns::deltaElement_t& );




    /**
    * �������� ���������� ��� ��������� ���������.
    */
    void statistics();




    /**
    * ������������ ����������� ��� �������, �������� �� ���������
    * ������ timeConverge().
    */
    void calcOptimalTimestep( int step );




    /**
    * @return ��������� ����� ��������� ���� �����.
    *         ������������� ����� ��������, ��� ����� ���������
    *         ���� �� �����.
    *         0.0 - ��� ����� ��� ������.
    *         ������������� - ��� ��� ����� ������� �� ���������.
    */
    static real_t estimateTimeApproaching(
        const pns::point_t& a,
        const pns::point_t& b
    );




    /**
    * �������� ���������� ������� �������.
    */
    void harvestStatisticsForOptimizeTimestep();




private:
    /**
    * ��������� ��� ����������� ��������.
    * ����������� � �������� ������ ������ ��� �����.
    */
    pns::statistics_t mStatistics;
};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte






#include "EngineCPU.inl"
