/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Hashing.h>

#include <cstdint>

namespace sk
{
    class cUUID_Helper;

    class alignas( 16 ) cUUID
    {
        // Don't make a full 128-bit integer value as the full functionality isn't required.
        uint64_t m_low_  = 0;
        uint64_t m_high_ = 0;

        friend class sk::cUUID_Helper;
    public:
        // It has constexpr support in case of future changes.
        constexpr cUUID() = default;
        constexpr cUUID( const uint64_t _first, const uint64_t _second )
        : m_low_( _first ), m_high_( _second )
        {}
        constexpr cUUID( const cUUID& _other ) = default;
        constexpr cUUID( cUUID&& _other )      = default;
        ~cUUID() = default;

        constexpr cUUID& operator=( const cUUID& _other ) = default;
        constexpr cUUID& operator=( cUUID&& _other )      = default;

        [[ nodiscard ]]
        constexpr auto get_low () const { return m_low_;  }
        [[ nodiscard ]]
        constexpr auto get_high() const { return m_high_; }

        constexpr auto operator<=>( const cUUID& _other ) const;

        [[ nodiscard ]]
        std::string to_string( bool _dashed = true ) const;
    };

    template<>
    struct hash< cUUID > : Hashing::iHashed
    {
        constexpr hash( const cUUID& _uuid ) // Might be safe enough?
        : iHashed( ( ( ( Hashing::val_64_const ^ _uuid.get_low() ) * Hashing::prime_64_const ) ^ _uuid.get_high() ) * Hashing::prime_64_const )
        {}

        HASH_REQUIREMENTS( hash )
    };

    // Defined here so helper isn't required to be included.
    extern cUUID GenerateRandomUUID();
} // sk::
