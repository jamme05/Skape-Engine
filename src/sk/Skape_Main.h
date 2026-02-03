/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

namespace sk::Runtime
{
    extern int  sk_main ( int, char** );
    extern void startup ( int, char** );
    extern bool run     ();
    extern void shutdown();
} // sk::Runtime::
