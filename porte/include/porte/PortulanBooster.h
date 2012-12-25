#pragma once

#include "Booster.h"
#include <portulan/portulan.h>
#include <typelib/typelib.h>
#include <memory>
#include <CL/cl_platform.h>



namespace porte {

template< size_t SX, size_t SY, size_t SZ >
class PortulanBooster;

}



namespace porte {


/**
* ������ ������ Portulan ��� ������������ �������� � ��������� 3D-����
* �� ����������� ����������� (OpenCL, Cuda).
*/
template< size_t SX, size_t SY, size_t SZ >
class PortulanBooster :
    public std::enable_shared_from_this< PortulanBooster< SX, SY, SZ > >,
    public Booster
{
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< PortulanBooster >  Ptr;
    typedef std::unique_ptr< PortulanBooster >  UPtr;

    
    /**
    * ��� ����������� �������.
    */
    typedef portulan::Portulan< SX, SY, SZ >  portulan_t;



public:
    /**
    * ������� ����� � ����-��������� (��. ����).
    */
    static const size_t TEMPERATURE_SIZE = SX * SY * SZ;


    /**
    * ��������� ��� ��������� ������ � ������.
    * ������ �������� � ����, ���������� ��� ��������� �������������
    * ����������� (���������� OpenCL, Cuda).
    */
    struct booster_t {
        /**
        * ������ ��� �������� ������������ ����������.
        *
        * @see Portulan::topology_t
        */
        cl_float temperature[ TEMPERATURE_SIZE ];



        /**
        * ��������, ��� ������ ��������� ���������������� � ����������.
        *//* - @todo optimize
        bool temperatureSynchronized;
        */
    };




    explicit PortulanBooster( portulan_t* );


    virtual ~PortulanBooster();




    portulan_t& portulan();
    portulan_t const& portulan() const;



    booster_t& booster();
    booster_t const& booster() const;




    /**
    * ������ ��� ������ ������ ��������� � ��������� booster_t.
    */
    virtual void toBooster();



    /**
    * ������ ��� ������ ������ �� ��������� booster_t � ��������.
    */
    virtual void fromBooster();





private:
    /**
    * ���������� �����.
    */
    portulan_t* mPortulan;


    /**
    * ��������� ��� ������� ������ � ������ (����������).
    */
    booster_t mBooster;

};




} // porte








#include "PortulanBooster.inl"
