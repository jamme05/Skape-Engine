/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

// TODO: Figure out in more detail what do do with error.
// TODO: At least make it call another function.

#define SK_FATAL( ... ) { std::println( stderr, __VA_ARGS__ ); SK_BREAK; std::exit( EXIT_FAILURE ); }

#define SK_WARNING( Severity, ... ) SK_CONST_PASSTHROUGH( Severity, std::println( stderr, __VA_ARGS__ ); )
