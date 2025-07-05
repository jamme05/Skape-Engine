/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#define RAW_FORMAT( Text, ... ) std::format( Text __VA_OPT__(,__VA_ARGS__) )
#define PRINT_FORMAT( Text, ... ) std::print( Text __VA_OPT__(,__VA_ARGS__) )
#define PRINTLN_FORMAT( Text, ... ) std::println( Text __VA_OPT__(,__VA_ARGS__) )
#define WARN_FORMAT( Text, ... ) std::println( stderr, Text __VA_OPT__(,__VA_ARGS__) )
#define ERR_FORMAT( Text, ... ) std::println( stderr, Text __VA_OPT__(,__VA_ARGS__) )
#define IS_FORMAT( ... ) ,1
#define FORMAT( Text, ... ) _FORMAT( Text, __VA_ARGS__ )

#define RAW_TEXT( ... ) std::format( __VA_ARGS__ )
#define PRINT_TEXT( ... ) std::print( __VA_ARGS__ )
#define PRINTLN_TEXT( ... ) std::println( __VA_ARGS__ )
#define WARN_TEXT( ... ) std::println( stderr, __VA_ARGS__ )
#define ERR_TEXT( ... ) std::println( stderr, __VA_ARGS__ )
#define TEXT( ... ) _TEXT( __VA_ARGS__ )
		
#define EXTRACT_FORMAT_TEXT( Text ) CONCAT( EXTRACT_FORMAT_TEXT_TYPE_, EXTRACT_FORMAT_TYPE( CONCAT( IS, Text ) ) ) ( Text )
