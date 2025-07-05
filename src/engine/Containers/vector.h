/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <vector>

#include <Containers/Allocator.h>

namespace sk
{
	// A vector using the memory tracker for allocation
	template< typename Ty >
	using vector = std::vector< Ty, allocator< Ty > >;
} // sk::