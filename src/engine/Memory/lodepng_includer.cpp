/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

// TODO: Move this file to a better place

#include "Memory/Tracker/Tracker.h"

void* lodepng_malloc(size_t size)
{
	return SK_ALLOC( size );
}
void* lodepng_realloc(void* ptr, size_t new_size)
{
	// TODO: Actual realloc
	void* tmp = SK_ALLOC( new_size );
	if( ptr != nullptr )
	{
		memcpy( tmp, ptr, new_size );
		SK_FREE( ptr );
	}
	return tmp;
}
void lodepng_free(void* ptr)
{
	SK_FREE( ptr );
}