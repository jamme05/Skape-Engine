/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Reflection/Types.h>
#include <Containers/Const/Linked_Array.h>

namespace sk::Reflection::Registry
{
    template< size_t I >
    extern cLinked_Array< type_pair_t > registered;
    template< size_t I >
    cLinked_Array< type_pair_t > registered = {};
} // sk::Reflection::Registry
