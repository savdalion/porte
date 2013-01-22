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
* ������ ��� ������������� *������� �������*.
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
    * ��������� ��������� ��������� �������� ���������, � �������� ��������
    * ������. ��������, EngineND ���������� ������� ������������� ��� ������
    * � ���. �����. ���� �� ����� �������� ���������� / �������� ���� ��
    * "������", ������� ������� sync().
    */
    virtual inline void sync() {
        // �� ��������� ������������� ���
    }

};


                } // l0
            } // starsystem
        } // dungeoncrawl
    } // world
} // porte







#include "Engine.inl"
