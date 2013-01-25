#pragma once

#include <assert.h>
#include <iostream>


namespace porte {


/**
* ����������.
*/
class Exception :
    public std::exception
{
public:
    inline Exception( const std::string& s ) :
        std::exception( s.c_str() )
    {
        assert( !s.empty()
            && "���� ������������� ���������� - ���� ������� ������." );
    }
};


} // porte
