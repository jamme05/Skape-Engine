/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#define STBI_MALLOC( S )     SK_ALLOC( S )
#define STBI_REALLOC( B, S ) SK_REALLOC( B, S )
#define STBI_FREE( B )       SK_FREE( B )

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>