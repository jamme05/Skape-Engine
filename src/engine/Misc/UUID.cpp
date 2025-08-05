/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "UUID.h"


namespace sk
{
    namespace
    {
        // https://en.cppreference.com/w/cpp/utility/format/spec.html
        // Is there any way to clean up the 16 uint8_t?...
        using uuid_format_string_t  = std::format_string< uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t >;
        constexpr uuid_format_string_t dashed_format_string { "{:0>2x}{:0>2x}{:0>2x}{:0>2x}-{:0>2x}{:0>2x}-{:0>2x}{:0>2x}-{:0>2x}{:0>2x}-{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}" };
        constexpr uuid_format_string_t compact_format_string{ "{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}" };
    } // ::

    std::string cUUID::to_string( const bool _dashed ) const
    {
        std::string_view target_string;

        if( _dashed )
            target_string = dashed_format_string.get();
        else
            target_string = compact_format_string.get();

        // I hate this, but I hope it works...
        const auto b = reinterpret_cast< const uint8_t* >( &m_low_ );

        return std::vformat(
            target_string,
            std::make_format_args(
                b[ 0 ], b[ 1 ], b[  2 ], b[  3 ], b[  4 ], b[  5 ], b[  6 ], b[  7 ],
                b[ 8 ], b[ 9 ], b[ 10 ], b[ 11 ], b[ 12 ], b[ 13 ], b[ 14 ], b[ 15 ]
            )
        );
    } // to_string
    
    constexpr auto cUUID::operator<=>( const cUUID& _other ) const
    {
        // Very much referenced from std::_Big_uint128
        if( m_low_ != _other.m_low_ )
            return m_low_ <=> _other.m_low_;

        return m_high_ <=> _other.m_high_;
    } // operator<=>
} // sk::
