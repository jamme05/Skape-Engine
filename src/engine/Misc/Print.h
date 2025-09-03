/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <iostream>

namespace sk
{
    namespace print_tools
    {
        // printf helpers.
        typedef unsigned long long llu_t;
        typedef signed   long long lli_t;
        typedef unsigned long       lu_t;
        typedef signed   long       li_t;
        typedef unsigned int         u_t;
        typedef signed   int         i_t;
    };

    void print_internal( std::FILE* const _stream, const std::string_view _out_str, const bool _add_newline );

    void println( std::FILE* const _stream );

    void println();

    template< class... Args >
    static void print( std::FILE* const _stream, const std::format_string< Args... > _fmt_str, Args&&... _args )
    {
        auto result_str = std::vformat( _fmt_str.get(), std::make_format_args( std::forward< Args >( _args )... ) );
        print_internal( _stream, result_str.c_str(), false );
    }

    template< class... Args >
    static void print( const std::format_string< Args... > _fmt_str, Args&&... _args )
    {
        auto result_str = std::vformat( _fmt_str.get(), std::make_format_args( std::forward< Args >( _args )... ) );
        print_internal( stdout, result_str, false );
    }

    template< class... Args >
    static void println( std::FILE* const _stream, const std::format_string< Args... > _fmt_str, Args&&... _args )
    {
        auto result_str = std::vformat( _fmt_str.get(), std::make_format_args( std::forward< Args >( _args )... ) );
        print_internal( _stream, result_str, true );
    }

    template< class... Args >
    static void println( const std::format_string< Args... > _fmt_str, Args&&... _args )
    {
        auto result_str = std::vformat( _fmt_str.get(), std::make_format_args( std::forward< Args >( _args )... ) );
        print_internal( stdout, result_str.c_str(), true );
    }
} // sk::

#define TO_LLU( Val ) static_cast< sk::print_tools::llu_t >( Val )
#define TO_LLI( Val ) static_cast< sk::print_tools::lli_t >( Val )
#define TO_LU( Val )  static_cast< sk::print_tools::lu_t  >( Val )
#define TO_LI( Val )  static_cast< sk::print_tools::li_t  >( Val )
#define TO_U( Val )   static_cast< sk::print_tools::u_t   >( Val )
#define TO_I( Val )   static_cast< sk::print_tools::i_t   >( Val )
