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


namespace pd = portulan::world::dungeoncrawl::starsystem::l0;


/**
* ������ ��� ������������� ������� �������.
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
    // @todo �������� 'extent' ��� ���� ������� ����.
    inline Engine( portulan_t* p, pd::real_t extent, pd::real_t timestep ) :
#if defined( OPENCL_STARSYSTEM_L0_ENGINE_PORTE )
        EngineOpenCL( p, timestep )
#elif defined( ND_STARSYSTEM_L0_ENGINE_PORTE )
        EngineND( p, extent, timestep )
#else
        EngineCPU( p, timestep )
#endif
    {
    }


    inline virtual ~Engine() {
    }



    /**
    * ��������� ��������� ��������� �������� ���������, � �������� ��������
    * ������. ��������, EngineND ���������� ������� ������������� ��� ������
    * � ���. �����. ���� �� ����� �������� ���������� / �������� ���� ��
    * "������", ������� ������� sync().
    */
    virtual inline void sync() {
        // �� ���������, ������������� ���
    }

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte
