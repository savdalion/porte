#pragma once

#include "../porte/Engine.h"



namespace porte {

template< size_t SX, size_t SY, size_t SZ >
class HeatTransfer;

}




namespace porte {


/**
* ������ �����������.
* ����� ���������� �������� ������ ����� � ����������� �� ���������� � �������
* � �������� �������.
*
* @template ��. Engine.
*/
template< size_t SX, size_t SY, size_t SZ >
class HeatTransfer : public Engine< SX, SY, SZ > {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< HeatTransfer >  Ptr;
    typedef std::unique_ptr< HeatTransfer >  UPtr;


public:
    virtual void operator()( portulan::Portulan3D< SX, SY, SZ >&,  int pulse );

};




} // porte








#include "HeatTransfer.inl"
