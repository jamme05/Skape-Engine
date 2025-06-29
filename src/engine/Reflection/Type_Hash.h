/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Hashing.h>

namespace sk
{
    template<>
    struct hash< void > : Hashing::iHashed
    {
        constexpr hash( void )
        : iHashed( 0 )
        {} // Invalid type
        constexpr hash( const str_hash& _str_hash )
        : iHashed( _str_hash.getValue() )
        {
        } // type_hash

        constexpr hash( const uint64_t _raw_value )
        : iHashed( _raw_value )
        {
        } // type_hash

        [[ deprecated( "Constructor with Name and Line is deprecated. Use the safer version with source location" ) ]]
        constexpr hash( const char* _str, const uint64_t _val )
        : iHashed( Hashing::fnv1a_64( _str, _val ) )
        {
        } // type_hash

        constexpr hash( const char* _name, const char* _namespace = "" )
        : iHashed( ( Hashing::fnv1a_64( _name ) ^ Hashing::fnv1a_64( _namespace ) ) * Hashing::prime_64_const )
        {
        } // type_hash

        HASH_REQUIREMENTS( hash )
    };

    typedef hash< void > type_hash;
} // qw
