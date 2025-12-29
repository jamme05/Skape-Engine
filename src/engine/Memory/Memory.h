/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace sk::Memory
{
	enum eAlignment : uint16_t
	{
		kDisableAlign = 1,
		kB16Align     = 16,   // 16-byte aka Vector 4 aligned.
		kShaderAlign  = kB16Align, // 16-byte aka Vector 4 aligned.
		kB32Align     = 32,   // 32-byte aligned
		kB64Align     = 64,   // 64-byte aligned
		kB128Align    = 128,  // 128-byte aligned
		kB256Align    = 256,  // 256-byte aligned
		kB512Align    = 512,  // 512-byte aligned
		kB1024Align   = 1024, // 1024-byte aligned
		kB2048Align   = 2048, // 2048-byte aligned
		kB4096Align   = 4096, // 4096-byte aligned
	};

	// TODO: Rename default_alignment to something more fitting the naming convention.
	/* Allows changing the engines running alignment. But the general default will still be named Default.
	 * NOTE: NEEDS TO BE a power of 2
	 */
	constexpr static eAlignment default_alignment = kB64Align;

	constexpr bool   is_power_of_two( const size_t _value ){ return ( _value & ( _value - 1 ) ) == 0; }
	// Returns the size ( in bytes ) aligned.
	constexpr size_t get_aligned( const size_t _size, const uint32_t _align = default_alignment )
	{
		if( is_power_of_two( _align ) )
		{
			const auto align = _align - 1;
			return ( _size + align ) & ~align;
		}

 		return _size + _size % _align;
	}
	// Modifies and returns the size ( in bytes ) aligned.
	constexpr size_t make_aligned( size_t& _size, const eAlignment _align = default_alignment ){ return _size = get_aligned( _size, _align ); }

	template< typename Ty, eAlignment Alignment = default_alignment >
	constexpr size_t get_size( const size_t _count = 1 )
	{
		const auto byte_size = sizeof( Ty ) * _count;
		if constexpr( Alignment == kDisableAlign ) return byte_size;
		else return get_aligned( byte_size, Alignment );
	} // get_size

	template< typename Ty, size_t Count = 1, eAlignment Alignment = default_alignment >
	static constexpr size_t byte_size = get_size< Ty, Alignment >( Count );
} // sk::Memory::

namespace sk::Memory::Internal
{
	// Un-tracked alloc, use only for internal stuff.
	// Use malloc for direct allocations.
	template< typename Ty, typename... Args >
	static Ty* alloc( Args... _args )
	{
		Ty* ptr = static_cast< Ty* >( ::malloc( byte_size< Ty, default_alignment > ) );

		::new( ptr ) Ty( std::forward< Args >( _args )... );

		return ptr;
	} // alloc

	// Unrackable free, use only for internal stuff.
	template< typename Ty >
	static void free( Ty* _block )
	{
		if( !_block )
			return;

		_block->~Ty();

		::free( _block );
	} // free

} // sk::Memory::Internal::

#define QW_INTERNAL_ALLOC( size ) sk::Memory::Internal::alloc( size )
#define QW_INTERNAL_FREE( block ) sk::Memory::Internal::free( block )
