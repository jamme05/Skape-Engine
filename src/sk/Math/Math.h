/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <bit>
#include <cstdint>
#include <math.h>
#include <numbers>
#include <valarray>

namespace sk
{
	template< class T >
	using enable_arithmetic_t = std::enable_if_t< std::is_arithmetic_v< T >, T >;
	template< class Ty, class Ty2 = Ty >
	concept enable_artimetic_c = std::is_arithmetic_v< Ty > && std::is_arithmetic_v< Ty2 >;
	// TODO: Find if there's a better way, like having two concepts with the same name.

	template< class T >
	struct nearest_float
	{
		typedef std::conditional_t< std::is_same_v< T, long double >, long double,
		std::conditional_t< std::is_same_v< T, double > || std::is_integral_v< T >, double, float > > type;
	};

	template< class T >
	using nearest_float_t = typename nearest_float< T >::type;

	template< class T1, class T2 >
	struct shared_nearest_float
	{
		typedef nearest_float_t< T1 > Ty1;
		typedef nearest_float_t< T2 > Ty2;

		typedef std::conditional_t< std::is_same_v< Ty1, long double > || std::is_same_v< Ty2, long double >, long double,
		        std::conditional_t< std::is_same_v< Ty1, double >      || std::is_same_v< Ty2, double >, double, float > > type;
	};

	template< class T1, class T2 >
	using common_float_type_t = typename shared_nearest_float< T1, T2 >::type;

	namespace Math
	{
		/*
		 * Function format:
		 *
		 * Code has to be on a single line unless using template or close/above to 150 characters long.
		 *
		 * template< TN... > [ T or return value ]
		 *    [ function_name ]( Args... ){ return [ optional: static_cast< T > ][ code ] }
		 *
		 * Code allowed to be multiline in case of typedef or single line too long.
		 *
		 * TODO: Chance format.
		 */

		template< std::floating_point Ty >
		constexpr static auto kPi  = std::numbers::pi_v< Ty >;
		template< std::floating_point Ty >
		constexpr static auto kPi2 = kPi< Ty > * kPi< Ty >;
		template< std::floating_point Ty >
		constexpr static auto kDegToRad = kPi< Ty > / static_cast< Ty >( 180.0 );
		template< std::floating_point Ty >
		constexpr static auto kRadToDeg = static_cast< Ty >( 180.0 ) / kPi< Ty >;

		// Math:
		template< class T > T
		constexpr square( const T _val ){ return _val * _val; }

		// Const pow variant.
		template< class T1 >
		requires std::is_arithmetic_v< T1 >
		consteval T1 cpow( T1 _val, uint32_t _exp )
		{
			// Based of: https://en.cppreference.com/w/cpp/language/if.html#Consteval_if
			if( _val == T1( 0 ) )
				return _val;

			T1 res{ 1 };
			while( _exp )
			{
				if( _exp & 1 ) res *= _val;
				_exp /= 2;
				_val *= _val;
			}
			return res;
		}

		// WARNING, if used during compile time, it's behaviour will be different. Check cpow.
		// For a powering 2 by anything, use pow2 instead.
		template< class T1, class T2 >
		requires ( enable_artimetic_c< T1, T2 > )
		constexpr T1 pow( const T1 _val, const T2 _exp )
		{
			if consteval
			{
				return cpow( _val, static_cast< uint32_t >( _exp ) );
			}
			typedef common_float_type_t< T1, T2 > type;
			return static_cast< T1 >( std::pow( type( _val ), type( _exp ) ) );
		}

		// Gets 2 powered by exp.
		template< std::integral Ty = uint8_t >
		constexpr Ty pow2( const uint8_t _exp )
		{
			return static_cast< Ty >( 0x1 ) << _exp;
		}
		
		// Returns the nearest power of two that's less than this value.
		template< std::integral Ty >
		constexpr Ty floorToPow2( const Ty _val )
		{
			if( _val <= Ty( 2 ) )
				return _val;
			
			static constexpr auto kBitSize = sizeof( Ty ) * 8;
			
			return pow2< Ty >( kBitSize - std::countl_zero( _val ) - 1 );
		}

		// Returns the nearest power of two.
		template< std::integral Ty >
		constexpr Ty roundToPow2( const Ty _val )
		{
			if( _val <= Ty( 2 ) )
				return _val;
			
			static constexpr auto kBitSize = sizeof( Ty ) * 8;
			
			const uint8_t bits = kBitSize - std::countl_zero( _val );
			
			// The defining bit is the bit behind the bit furthest to the right.
			// If the number has this bit, it means that we will ciel, otherwise we floor.
			auto defining_bit = pow2< Ty >( bits - 2 );
			
			return ( ( _val & defining_bit ) == 0 ) ? pow2< Ty >( bits - 1 ) : pow2< Ty >( bits );
		}

		// Returns the nearest power of two that's greater than this value.
		template< std::integral Ty >
		constexpr Ty ceilToPow2( Ty _val )
		{
			if( _val <= Ty( 2 ) )
				return _val;
			
			--_val;
			
			static constexpr auto kBitSize = sizeof( Ty ) * 8;

			return pow2< Ty >( kBitSize - std::countl_zero( _val ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T cos( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::cos( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T sin( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::sin( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T tan( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::tan( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T acos( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::cos( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T asin( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::sin( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T atan( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::tan( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		T sqrt( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::sqrt( type( _val ) ) );
		}

		// Logical
		template< class T >
		requires enable_artimetic_c< T >
		constexpr T abs( const T _val )
		{
			if consteval
			{
				// No need to calculate anything if it can't be unsigned.
				if constexpr( std::is_unsigned_v< T > )
					return _val;
				return _val < 0 ? -_val : _val;
			}

			// Default std
			typedef nearest_float_t< T > type;
			return static_cast< T >( std::abs( type( _val ) ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		constexpr T	degToRad( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( kDegToRad< type > * type( _val ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		constexpr T	radToDeg( const T _val )
		{
			typedef nearest_float_t< T > type;
			return static_cast< T >( kRadToDeg< type > * type( _val ) );
		}

		template< class T >
		requires enable_artimetic_c< T >
		constexpr T	min( const T _a, const T _b )
		{
			return _a < _b ? _a : _b;
		}

		template< class T >
		requires enable_artimetic_c< T >
		constexpr T	max( const T _a, const T _b )
		{
			return _a > _b ? _a : _b;
		}

	} // Math::
} // sk::