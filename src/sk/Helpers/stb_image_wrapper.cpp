/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

// ReSharper disable CppUnusedIncludeDirective

#include <Memory/Tracker/Tracker.h>

#define STBI_MALLOC( S )     sk::Memory::Tracker::alloc( S )
#define STBI_REALLOC( B, S ) sk::Memory::Tracker::realloc( B, S )
#define STBI_FREE( B )       sk::Memory::free( static_cast< void* >( B ) )

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>