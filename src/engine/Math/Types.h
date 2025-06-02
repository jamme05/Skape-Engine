/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

// TODO: Requires checks if it's numeric
template <typename T, typename T2> qw::cVector2<T> operator*(T2 _i, qw::cVector2<T> _v)
{
	return _v * static_cast<T>(_i);
}

template <typename T, typename T2> qw::cVector3<T> operator*(T2 _i, qw::cVector3<T> _v)
{
	return _v * static_cast<T>(_i);
}

template <typename T, typename T2>
qw::cVector4<T> operator*(T2 _i, qw::cVector4<T> _v)
{
	return _v * static_cast<T>(_i);
}