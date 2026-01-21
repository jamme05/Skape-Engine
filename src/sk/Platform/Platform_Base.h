/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

// Will be overriden by Module.

namespace sk::Platform
{
    typedef void( *function_ptr_t )();
    extern function_ptr_t get_proc_address( const char* _name );
} // sk::Platform
