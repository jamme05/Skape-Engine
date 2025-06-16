/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <vector>

#include "allocator.h"

namespace sk
{
	template< typename Ty >
	using vector = std::vector< Ty, allocator< Ty > >;
} // sk::