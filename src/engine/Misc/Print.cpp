/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Print.h"

namespace sk
{
    void print_internal( std::FILE* const _stream, const std::string_view _out_str, const bool _add_newline )
    {
        (void)std::fprintf( _stream, _add_newline ? "%s\n" : "%s", _out_str.data() );
    }

    void println( std::FILE* const _stream )
    {
        (void)std::fprintf( _stream, "\n" );
    }

    void println()
    {
        println( stdout );
    }
} // sk::
