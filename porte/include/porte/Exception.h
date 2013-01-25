#pragma once

#include <assert.h>
#include <iostream>


namespace porte {


/**
* Исключения.
*/
class Exception :
    public std::exception
{
public:
    inline Exception( const std::string& s ) :
        std::exception( s.c_str() )
    {
        assert( !s.empty()
            && "Если выбрасывается исключение - надо сказать почему." );
    }
};


} // porte
