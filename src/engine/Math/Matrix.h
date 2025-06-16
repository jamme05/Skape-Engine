/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */


#pragma once

#include "Vector.h"
#include "Vector2.h"

namespace sk::Math
{
	template < size_t W, size_t H, typename T > class cMatrix
	{
	public:
		cVector< W, T > m_matrix[ H ];

		cVector< W, T >& operator[]( size_t _r ){ return m_matrix[ _r ]; }
	};
} // sk::