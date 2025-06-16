/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Type_Registry.h"

REGISTER_TYPE  ( bool, "Boolean" )
REGISTER_T_TYPE( int8, "Char" )
REGISTER_T_TYPE( uint8, "Byte" )
REGISTER_T_TYPE( int16, "Short" )
REGISTER_T_TYPE( uint16, "Unsigned Short" )
REGISTER_T_TYPE( int32, "Integer" )
REGISTER_T_TYPE( uint32, "Unsigned Integer" )
REGISTER_T_TYPE( int64, "Long" )
REGISTER_T_TYPE( uint64, "Unsigned Long" )

REGISTER_TYPE( float, "Float" )
REGISTER_TYPE( double, "Double" )
