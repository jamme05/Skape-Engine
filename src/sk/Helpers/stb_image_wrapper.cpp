/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

// ReSharper disable CppUnusedIncludeDirective

#include <sk/Memory/Tracker/Tracker.h>

#define STBI_MALLOC( S )     SK_ALLOC( S )
#define STBI_REALLOC( B, S ) SK_REALLOC( B, S )
#define STBI_FREE( B )       SK_FREE( static_cast< void* >( B ) )

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>