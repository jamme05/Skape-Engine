/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Misc/Hashing.h>

namespace sk
{
    template<>
    struct hash< void >
    {
        constexpr hash( void )
        : m_hash_( 0 )
        {} // Invalid type
        
        constexpr hash( const str_hash& _str_hash )
        : m_hash_( _str_hash.value() )
        {
        } // type_hash

        constexpr hash( const uint64_t _raw_value )
        : m_hash_( _raw_value )
        {
        } // type_hash

        [[ deprecated( "Constructor with Name and Line is deprecated. Use the safer version with source location" ) ]]
        constexpr hash( const char* _str, const uint64_t _val )
        : m_hash_( Hashing::fnv1a_64( _str, _val ) )
        {
        } // type_hash

        constexpr hash( const char* _name, const char* _namespace = "" )
        : m_hash_( ( Hashing::fnv1a_64( _name ) ^ Hashing::fnv1a_64( _namespace ) ) * Hashing::prime_64_const )
        {
        } // type_hash

        HASH_REQUIREMENTS( hash )
    };

    typedef hash< void > type_hash;
} // qw
