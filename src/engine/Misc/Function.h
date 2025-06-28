/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <tuple>
#include <type_traits>

namespace sk
{
    // Direct forward to std::apply, just for a consistent namespace usage. Could be nice for future changes as well.
    template< class Call, class Tuple >
    constexpr decltype( auto ) apply( Call&& _callable, Tuple&& _tuple ) noexcept
    {
        return std::apply( std::forward< Call >( _callable ), std::forward< Tuple >( _tuple ) );
    }

    template< class Call, class Ty, class Tuple, size_t... Indices >
    constexpr decltype( auto ) unwrap_inst_apply( Call&& _callable, Ty&& _instance, Tuple&& _tuple, std::index_sequence< Indices... > )
    {
        std::invoke( std::forward< Call >( _callable ), std::forward< Ty >( _instance ), std::get< Indices >( std::forward< Tuple >( _tuple ) )... );
    } // unwrap_apply

    // Variant of apply which allows you to give it an instance.
    template< class Ty, class Call, class Tuple >
    constexpr decltype( auto ) apply( Call&& _callable, Ty&& _instance, Tuple&& _tuple ) noexcept
    {
        return unwrap_inst_apply( std::forward< Call >( _callable ), std::forward< Ty >( _instance ), std::forward< Tuple >( _tuple ),
            std::make_index_sequence< std::tuple_size_v< std::remove_reference_t< Tuple > > >{} );
    }
} // sk::
