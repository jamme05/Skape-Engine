/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <print>

namespace sk::Severity
{
    enum : uint32_t
    {
        kValueMask =  static_cast< uint32_t >( std::numeric_limits< uint16_t >::max() ),
        kTypeMask  = ~kValueMask,

        // A way to use general in case it isn't specified.
        kAny        = 0,
        kGeneral    = 1 << 16,
        kEngine     = 1 << 17,
        kGraphics   = 1 << 18,
        kPlatform   = 1 << 19,
        kEditor     = 1 << 20,
        kMemory     = 1 << 21,
        kReflection = 1 << 22,

        kAllValues = kValueMask,
        kAllTypes  = kTypeMask,
        kAll = std::numeric_limits< uint32_t >::max(),
    };
    static constexpr auto GetValue( const uint32_t _severity ) noexcept { return kValueMask & _severity; }
    static constexpr auto GetType ( const uint32_t _severity ) noexcept { return kTypeMask & _severity; }

    // TODO: Add function to math as nearest power of two.
    // https://stackoverflow.com/a/466242

    // Creates a mask allowing any value between 0 and _value. NOTE that it will a few above the value.
    static constexpr uint32_t Range( uint_fast16_t _value )
    {
        for( int_fast8_t s = 16; s >= 0; --s )
            _value |= _value >> s;
                
        return static_cast< uint32_t >( _value );
    }

    static constexpr uint32_t kSeverityFilter = kAll; // All severities on for now.
    static constexpr uint32_t kSeverityValue  = GetValue( kSeverityFilter ) == 0 ? kAllTypes : GetValue( kSeverityFilter );
    static constexpr uint32_t kSeverityType   = GetType ( kSeverityFilter );

    static constexpr bool IsEnabled( const uint32_t _severity ) noexcept
    {
        const auto value = GetValue( _severity ) | kSeverityValue;
        auto       type  = GetType ( _severity );
        if( type == kAny )
            type = kGeneral;

        return ( value <= kSeverityValue ) && ( type & kSeverityType );
    }
} // sk::Severity::

// TODO: Add runtime variant.
#define SK_CONST_PASSTHROUGH( SeverityValue, ... ) \
    if constexpr( sk::Severity::IsEnabled( SeverityValue ) ){ \
    __VA_ARGS__ };
