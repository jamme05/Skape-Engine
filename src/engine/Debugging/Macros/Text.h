/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Debugging/Macros/Warning.h>

#define RAW_TEXT( ... ) std::format( __VA_ARGS__ )
#define PRINT_TEXT( ... ) std::print( __VA_ARGS__ )
#define PRINTLN_TEXT( ... ) std::println( __VA_ARGS__ )
#define WARN_TEXT( ... ) std::println( stderr, __VA_ARGS__ );
#define ERR_TEXT( ... ) SK_FATAL( __VA_ARGS__ )
#define TEXT( ... ) _TEXT( __VA_ARGS__ )
		
#define EXTRACT_FORMAT_TEXT( Text ) CONCAT( EXTRACT_FORMAT_TEXT_TYPE_, EXTRACT_FORMAT_TYPE( CONCAT( IS, Text ) ) ) ( Text )
