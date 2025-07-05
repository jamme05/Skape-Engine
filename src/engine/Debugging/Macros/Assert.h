/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

// TODO: Maybe move to its own file?
// If the Condition is true, an error message will be printed and the function will return.
#define SK_WARN_RET_IF( Condition, Text, ... ) if( Condition ){ [[ unlikely ]] CONCAT( WARN, Text ); return __VA_ARGS__; }
// If the Condition is false, an error message will be printed and the function will return.
#define SK_WARN_RET_IFN( Condition, Text, ... ) SK_WARN_RET_IF( !( Condition ), Text, __VA_ARGS__ )
