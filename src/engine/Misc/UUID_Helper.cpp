/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "UUID_Helper.h"

#include <Misc/UUID.h>

namespace sk
{
    namespace
    {
        uint8_t flip_hex( const uint8_t _value )
        {
            static constexpr uint8_t low_mask  = 0xf0;
            static constexpr uint8_t high_mask = 0x0f;

            uint8_t low  = _value & low_mask;
            uint8_t high = _value & high_mask;

            std::swap( low, high );

            return low | high;
        }

        constexpr bool    should_flip        = std::endian::native == std::endian::little;
        // We're working in a Byte environment hence we need to convert it.
        constexpr uint8_t uuid_version_index = 48 / 8;
        constexpr uint8_t uuid_variant_index = 64 / 8;
        constexpr uint8_t uuid_version_mask  = 0b00001111;
        constexpr uint8_t uuid_variant_mask  = 0b00000000;

        // Stay with the RFC 9562 standard ( https://www.rfc-editor.org/rfc/rfc9562.txt ). 
        constexpr uint8_t uuid_v4_version = 0b01000000;
        constexpr uint8_t uuid_v4_variant = 0b00100000;
    } // ::

    cUUID_Helper::cUUID_Helper()
    : m_random_generator_( reinterpret_cast< uint64_t >( __TIME__ ) )
    {
    } // cUUID_Helper

    cUUID cUUID_Helper::GenerateRandomUUID()
    {
        auto low  = m_random_generator_();
        auto high = m_random_generator_();

        if constexpr( should_flip )
        {
            low  = std::byteswap( low );
            high = std::byteswap( high );
        }

        cUUID uuid{ low, high };
        const auto raw = reinterpret_cast< uint8_t* >( &uuid );

        // Add the version and variant.
        raw[ uuid_version_index ] &= uuid_version_mask;
        raw[ uuid_version_index ] |= uuid_v4_version;

        raw[ uuid_variant_index ] &= uuid_variant_mask;
        raw[ uuid_variant_index ] |= uuid_v4_variant;

        return uuid;
    } // GenerateRandomUUID

    cUUID GenerateRandomUUID()
    {
        return cUUID_Helper::get().GenerateRandomUUID();
    } // GenerateRandomUUID

} // sk::
