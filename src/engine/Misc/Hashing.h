/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "Math/Math.h"

// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out
// e.g. hash_32_fnv1a_const("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/
// "inspiration" found at: https://gist.github.com/ruby0x1/81308642d0325fd386237cfa3b44785c

namespace Hashing
{
	constexpr uint32_t val_32_const   = 0x811c9dc5;
	constexpr uint32_t prime_32_const = 0x1000193;
	constexpr uint64_t val_64_const   = 0xcbf29ce484222325;
	constexpr uint64_t prime_64_const = 0x100000001b3;

	constexpr uint32_t fnv1a_32( const char* const _s, const uint32_t _v = val_32_const ) noexcept {
		return ( _s[ 0 ] == '\0' ) ? _v : fnv1a_32( &_s[ 1 ], ( _v ^ static_cast< uint32_t >( static_cast< uint8_t >( _s[ 0 ] ) ) ) * prime_32_const );
	}

	constexpr uint64_t fnv1a_64( const char* const _s, const uint64_t _v = val_64_const ) noexcept {
		return ( _s[ 0 ] == '\0' ) ? _v : fnv1a_64( &_s[ 1 ], ( _v ^ static_cast< uint64_t >( static_cast< uint8_t >( _s[ 0 ] ) ) ) * prime_64_const );
	}

	constexpr uint32_t fnv1a_32s( const char* const _s, const size_t _c, const size_t _i = 0, const uint32_t _v = val_32_const ) noexcept {
		return ( _i >= _c ) ? _v : fnv1a_32s( _s, _c, _i + 1, ( _v ^ static_cast< uint32_t >( static_cast< uint8_t >( _s[ _i ] ) ) ) * prime_32_const );
	}

	constexpr uint64_t fnv1a_64s( const char* const _s, const size_t _c, const size_t _i = 0, const uint64_t _v = val_64_const ) noexcept {
		return ( _i >= _c ) ? _v : fnv1a_64s( _s, _c, _i + 1, ( _v ^ static_cast< uint64_t >( static_cast< uint8_t >( _s[ _i ] ) ) ) * prime_64_const );
	}

	inline uint32_t fnv1a_32( const std::string& _s ) noexcept {
		return fnv1a_32( _s.c_str() );
	}

	inline uint64_t fnv1a_64( const std::string& _s ) noexcept {
		return fnv1a_64( _s.c_str() );
	}


	template< class Ty >
	constexpr uint32_t fnv1a_32( const Ty& _in, const uint32_t _v = val_32_const ) noexcept {
		return fnv1a_32s( reinterpret_cast< const char* >( &_in ), sizeof( _in ), _v );
	}

	template< class Ty >
	constexpr uint64_t fnv1a_64( const Ty& _in, const uint64_t _v = val_64_const ) noexcept {
		return fnv1a_64s( reinterpret_cast< const char* >( &_in ), sizeof( _in ), _v );
	}
	template< class Ty >
	constexpr uint32_t fnv1a_32( const Ty* _in, const uint32_t _v = val_32_const ) noexcept {
		return fnv1a_32s( reinterpret_cast< const char* >( _in ), sizeof( _in ), _v );
	}

	template< class Ty >
	constexpr uint64_t fnv1a_64( const Ty* _in, const uint64_t _v = val_64_const ) noexcept {
		return fnv1a_64s( reinterpret_cast< const char* >( _in ), sizeof( _in ), _v );
	}

#define HASH_REQUIREMENTS( Class ) \
constexpr Class( const Class& _other ) : m_hash_( _other.m_hash_ ){}; \
constexpr Class( Class&& _other ) noexcept : m_hash_( std::move( _other.m_hash_ ) ){} \
~Class( void ) = default; \
constexpr auto value() const { return m_hash_; } \
constexpr Class& operator=( const Class& ) = default; \
constexpr Class& operator=( Class&& _other ) noexcept = default; \
constexpr bool   operator==( const Class& _other ) const { return m_hash_ == _other.m_hash_; } \
constexpr bool   operator!=( const Class& _other ) const { return m_hash_ != _other.m_hash_; } \
constexpr auto   operator<=>( const Class & _other ) const { return m_hash_ <=> _other.m_hash_; } \
private: \
uint64_t m_hash_;

} // Hashing

namespace sk
{
	template< class Ty >
	struct hash
	{
		constexpr hash( const Ty& _to_hash )
		: m_hash_( fnv1a_64< Ty >( _to_hash ) ){}

		HASH_REQUIREMENTS( hash )
	};

	template<>
	struct hash< char >
	{
		constexpr hash() : m_hash_( 0 ){}

		constexpr hash( const std::string_view& _to_hash )
		: hash( _to_hash.data(), _to_hash.size() )
		{
		} // hash
		
		constexpr hash( const std::string& _to_hash )
		: hash( _to_hash.data(), _to_hash.size() )
		{
 		} // hash

		hash( const std::filesystem::path& _to_hash )
		: m_hash_( Hashing::fnv1a_64( _to_hash.c_str() ) )
		{
		} // hash

		constexpr hash( const std::pmr::string& _to_hash )
		: hash( _to_hash.c_str(), _to_hash.size() )
		{
		}

		constexpr hash( const char* _to_hash )
		: m_hash_( _to_hash ? Hashing::fnv1a_64( _to_hash ) : 0 )
		{
		}

		constexpr hash( const char* _to_hash, const size_t _c )
		: m_hash_( Hashing::fnv1a_64s( _to_hash, _c ) )
		{
		}

		constexpr hash( const char* _to_hash, const int64_t _c )
		: m_hash_( 0 )
		{
			if( _c < 0 )
				Hashing::fnv1a_64( _to_hash );
			else
				Hashing::fnv1a_64s( _to_hash, _c );
		}

		static const hash kEmpty;

		HASH_REQUIREMENTS( hash )
	};
	constexpr hash< char > hash< char >::kEmpty = {};

	// TODO: Rename to str_hash_t
	typedef hash< char > str_hash;

	template<>
	struct hash< uint32_t >
	{
		constexpr hash( const uint32_t& _to_hash )
		: m_hash_( static_cast< uint64_t >( _to_hash ) ){}

		HASH_REQUIREMENTS( hash )
	};

	template<>
	struct hash< uint64_t >
	{
		constexpr hash( const uint64_t& _to_hash )
		: m_hash_( _to_hash ){}

		HASH_REQUIREMENTS( hash )
	};
} // sk::

template< class Ty >
struct std::hash< sk::hash< Ty > >
{
	uint64_t operator()( const sk::hash< Ty >& _hash ) const
	{
		return _hash.value();
	}
};