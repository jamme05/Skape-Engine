/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once
#include <algorithm>

namespace sk
{
    template< class Ty >
    consteval Ty&& Const( Ty&& _v ){ return std::forward< Ty >( _v ); }
} // sk::
