#pragma once

#include "Booster.h"
#include <portulan/portulan.h>
#include <typelib/typelib.h>
#include <memory>
#include <CL/cl_platform.h>



namespace porte {

template< size_t SX, size_t SY, size_t SZ >
class Portulan3DBooster;

}



namespace porte {


/**
* ������ ������ Portulan3D ��� ������������ �������� � ��������� 3D-����
* �� ����������� ����������� (OpenCL, Cuda).
*/
template< size_t SX, size_t SY, size_t SZ >
class Portulan3DBooster : public Booster {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< Portulan3DBooster >  Ptr;
    typedef std::unique_ptr< Portulan3DBooster >  UPtr;

    
    /**
    * ��� ����������� �������.
    */
    typedef portulan::Portulan3D< SX, SY, SZ >  portulan_t;



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
        */
        cl_float temperature[ TEMPERATURE_SIZE ];

        /**
        * ��������, ��� ������ ��������� ���������������� � ����������.
        *//* - @todo optimize
        bool temperatureSynchronized;
        */
    };




    Portulan3DBooster( portulan_t* );


    virtual ~Portulan3DBooster();




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








#include "Portulan3DBooster.inl"
