/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include <sk/Reflection/Types.h>

REGISTER_TYPE( sk::cVector2f, "Vector 2 ( float )" )
REGISTER_TYPE( sk::cVector2d, "Vector 2 ( double )" )
REGISTER_TYPE( sk::cVector2i32, "Vector 2 ( int )" )
REGISTER_TYPE( sk::cVector2u32, "Vector 2 ( unsigned int )" )

REGISTER_TYPE( sk::cVector3f, "Vector 3 ( float )" )
REGISTER_TYPE( sk::cVector3d, "Vector 3 ( double )" )
REGISTER_TYPE( sk::cVector3i32, "Vector 3 ( int )" )
REGISTER_TYPE( sk::cVector3u32, "Vector 3 ( unsigned int )" )

// TODO: Create a custom color class.
// REGISTER_TYPE( sk::cColor,    "Vector 3 ( float )" )
REGISTER_TYPE( sk::cVector4f, "Vector 4 ( float )" )
REGISTER_TYPE( sk::cVector4d, "Vector 4 ( double )" )
REGISTER_TYPE( sk::cVector4i32, "Vector 4 ( int )" )
REGISTER_TYPE( sk::cVector4u32, "Vector 4 ( unsigned int )" )

REGISTER_TYPE( sk::cMatrix4x4f, "Matrix 4x4 ( float )" )
REGISTER_TYPE( sk::cMatrix4x4d, "Matrix 4x4 ( double )" )

template< class Ty, size_t R, size_t C >
struct sk::get_type_info< sk::Math::cMatrix< R, C, Ty > > : sk::template_type_info
{
	constexpr static auto kPartTypeName = string< 128 >( sk::get_type_info< Ty >::kInfo.name );
	constexpr static auto kName         = str::concat< "sk::Math::cMatrix<", to_string( R ), ",", to_string( C ), ",", kPartTypeName, ">" >();
	constexpr static auto kPrettyName   = str::concat< "Matrix ", to_string( R ), "x", to_string( C ), " ( ", kPartTypeName, " )" >();
	constexpr static auto kInfo = sType_Info{
		.type = sType_Info::eType::kStandard, .hash{ kPrettyName.get() }, .size = sizeof( sk::cMatrix4x4d ),
		.name = kPrettyName.get(), .raw_name = kName.get()
	};
	constexpr static bool kValid = true;
	inline const static auto type_registry = sk::Reflection::cType_Manager::RegisterType( &kInfo );
};

// TODO: Requires checks if it's numeric
template <typename T, typename T2>
requires std::is_arithmetic_v< T >
sk::Math::cVector2<T> operator*(T2 _i, sk::Math::cVector2<T> _v)
{
	return _v * static_cast<T>(_i);
}

template <typename T, typename T2>
requires std::is_arithmetic_v< T2 >
sk::Math::cVector3<T> operator*(T2 _i, sk::Math::cVector3<T> _v)
{
	return _v * static_cast<T>(_i);
}

template <typename T, typename T2>
requires std::is_arithmetic_v< T2 >
sk::Math::cVector4<T> operator*(T2 _i, sk::Math::cVector4<T> _v)
{
	return _v * static_cast<T>(_i);
}
