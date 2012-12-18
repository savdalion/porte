#pragma once

#include "../../configure.h"
#include <limits>
#include <memory>



namespace porte {


/**
* Общий класс для создания движков портуланов (3D-карт).
*
* @see EngineWithoutBooster
*/
class AEngine {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< AEngine >  Ptr;
    typedef std::unique_ptr< AEngine >  UPtr;
    typedef AEngine*                    Ref;
};



} // porte
