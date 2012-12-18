#pragma once

#include "../../configure.h"
#include <limits>
#include <memory>



namespace porte {


/**
* ����� ����� ��� �������� ������� ���������� (3D-����).
*
* @see EngineWithoutBooster
*/
class AEngine {
public:
    /**
    * ������.
    */
    typedef std::shared_ptr< AEngine >  Ptr;
    typedef std::unique_ptr< AEngine >  UPtr;
    typedef AEngine*                    Ref;
};



} // porte
