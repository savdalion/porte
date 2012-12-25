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
* Обёртка класса Portulan для эффективного хранения и обработки 3D-карт
* на графических процессорах (OpenCL, Cuda).
*/
template< size_t SX, size_t SY, size_t SZ >
class PortulanBooster :
    public std::enable_shared_from_this< PortulanBooster< SX, SY, SZ > >,
    public Booster
{
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< PortulanBooster >  Ptr;
    typedef std::unique_ptr< PortulanBooster >  UPtr;

    
    /**
    * Тип ускоряемого объекта.
    */
    typedef portulan::Portulan< SX, SY, SZ >  portulan_t;



public:
    /**
    * Размеры полей в буст-структуре (см. ниже).
    */
    static const size_t TEMPERATURE_SIZE = SX * SY * SZ;


    /**
    * Структура для ускорения работы с картой.
    * Данные хранятся в виде, подходящем для обработки параллельными
    * алгоритмами (технологии OpenCL, Cuda).
    */
    struct booster_t {
        /**
        * Данные для передачи параллельным алгоритмам.
        *
        * @see Portulan::topology_t
        */
        cl_float temperature[ TEMPERATURE_SIZE ];



        /**
        * Признаки, что данные структуры синхронизированы с портуланом.
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
    * Методы для записи данных портулана в структуру booster_t.
    */
    virtual void toBooster();



    /**
    * Методы для записи данных из структуры booster_t в портулан.
    */
    virtual void fromBooster();





private:
    /**
    * Ускоряемый класс.
    */
    portulan_t* mPortulan;


    /**
    * Структура для быстрой работы с картой (портуланом).
    */
    booster_t mBooster;

};




} // porte








#include "PortulanBooster.inl"
