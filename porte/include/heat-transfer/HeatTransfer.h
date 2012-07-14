#pragma once

#include "../porte/Engine.h"



namespace porte {

template< size_t SX, size_t SY, size_t SZ >
class HeatTransfer;

}




namespace porte {


/**
* Движок теплообмена.
* Карта температур меняется каждый пульс в зависимости от температур и веществ
* в соседних ячейках.
*
* @template См. Engine.
*/
template< size_t SX, size_t SY, size_t SZ >
class HeatTransfer : public Engine< SX, SY, SZ > {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< HeatTransfer >  Ptr;
    typedef std::unique_ptr< HeatTransfer >  UPtr;


public:
    virtual void operator()( portulan::Portulan3D< SX, SY, SZ >&,  int pulse );

};




} // porte








#include "HeatTransfer.inl"
