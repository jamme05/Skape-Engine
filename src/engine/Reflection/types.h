/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#define SK_ALLOW_DIRECT_REGISTRY_ACCESS
#include "Type_Registry.h"
#undef SK_ALLOW_DIRECT_REGISTRY_ACCESS

// Register Default Types.
REGISTER_TYPE_INLINE( bool, "Boolean" )
REGISTER_T_TYPE_INLINE( int8, "Char" )
REGISTER_T_TYPE_INLINE( uint8, "Byte" )
REGISTER_T_TYPE_INLINE( int16, "Short" )
REGISTER_T_TYPE_INLINE( uint16, "Unsigned Short" )
REGISTER_T_TYPE_INLINE( int32, "Integer" )
REGISTER_T_TYPE_INLINE( uint32, "Unsigned Integer" )
REGISTER_T_TYPE_INLINE( int64, "Long" )
REGISTER_T_TYPE_INLINE( uint64, "Unsigned Long" )

REGISTER_TYPE_INLINE( float, "Float" )
REGISTER_TYPE_INLINE( double, "Double" )

// Register hash Types.
REGISTER_TYPE_INLINE( sk::type_hash, "Type Hash" )
REGISTER_TYPE_INLINE( sk::str_hash,  "String Hash" )
