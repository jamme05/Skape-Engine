/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#define RAW_FORMAT( Text, ... ) std::format( Text __VA_OPT__(,__VA_ARGS__) )
#define PRINT_FORMAT( Text, ... ) std::print( Text __VA_OPT__(,__VA_ARGS__) )
#define PRINTLN_FORMAT( Text, ... ) std::println( Text __VA_OPT__(,__VA_ARGS__) )
#define IS_FORMAT( ... ) ,1
#define FORMAT( Text, ... ) _FORMAT( Text, __VA_ARGS__ )
		
#define EXTRACT_FORMAT_TEXT_TYPE_1( Text ) Text
#define EXTRACT_FORMAT_TEXT_TYPE_0( Text ) _FORMAT( Text )
#define EXTRACT_FORMAT_TYPE( ... ) SECOND( __VA_ARGS__, 0 )
#define EXTRACT_FORMAT_TEXT( Text ) CONCAT( EXTRACT_FORMAT_TEXT_TYPE_, EXTRACT_FORMAT_TYPE( CONCAT( IS, Text ) ) ) ( Text )
