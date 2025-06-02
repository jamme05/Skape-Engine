/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Hashing.h>

namespace qw
{
    template<>
    class hash< void > : public Hashing::iHashed
    {
    public:
        constexpr hash( void )
        : iHashed( 0 )
        {} // Invalid type
        constexpr hash( const str_hash& _str_hash )
        : iHashed( _str_hash.getHash() )
        {
        } // type_hash

        constexpr hash( const uint64_t _raw_value )
        : iHashed( _raw_value )
        {
        } // type_hash
        constexpr hash( const char* _str, const uint64_t _val )
        : iHashed( Hashing::fnv1a_64( _str, _val ) )
        {
        } // type_hash


        HASH_REQUIREMENTS( hash )
    };

    typedef hash< void > type_hash;

}
