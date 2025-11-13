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
        // This is following the RFC 9562 standard for the UUIDv4 ( https://www.rfc-editor.org/rfc/rfc9562.html ). 
        // We're working in a Byte environment hence we need to convert it.
        constexpr uint8_t uuid_version_index = 48 / 8;
        constexpr uint8_t uuid_variant_index = 64 / 8;
        constexpr uint8_t uuid_version_mask  = 0b00001111;
        constexpr uint8_t uuid_variant_mask  = 0b00111111;

        constexpr uint8_t uuid_v4_version = 0b01000000;
        constexpr uint8_t uuid_v4_variant = 0b10000000;
    } // ::

    cUUID_Helper::cUUID_Helper()
    : m_random_generator_( m_random_device_() )
    {
    } // cUUID_Helper

    cUUID cUUID_Helper::GenerateRandomUUID()
    {
        const auto low  = m_random_generator_();
        const auto high = m_random_generator_();

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
        return cUUID_Helper::try_init().GenerateRandomUUID();
    } // GenerateRandomUUID

} // sk::
