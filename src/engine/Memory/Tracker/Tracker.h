/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Memory/Memory.h"
#include "Misc/Singleton.h"

#include <unordered_set>
#include <mutex>
#include <source_location>

#include <Macros/Enum_Builder.h>

namespace sk::Memory
{
	namespace Tracker
	{
		// Decides if the memory tracker should keep track of the entire history and not just the latest modification.
		// TODO: Check if memory history actually works.
		constexpr bool kSaveMemoryHistory = false;

		struct sMem_header
		{
			size_t item_size;
			size_t item_count;
		};

		// Can track source usage
		extern void*  alloc  ( size_t _size, const std::source_location& _location = std::source_location::current() );
		extern void*  realloc( void* _ptr, size_t _size, const std::source_location& _location = std::source_location::current() );
		extern void   free   ( void* _block, const std::source_location& _location = std::source_location::current() );
		extern size_t max_heap_size( void );

		extern void   init    ( void );
		extern void   shutdown( void );

	} // Tracker::

	// TODO: Move the extern functions to a different include to not have them be depending on the memory tracker.
	// This will also allow the singletons to be tracked.
	extern void* alloc_fast  ( size_t _size, eAlignment _alignment = default_alignment );
	extern void  free_fast   ( void*  _block );
	extern void* realloc_fast( void* _block, size_t _size, eAlignment _alignment = default_alignment );

	template< typename Ty, typename... Args >
	requires std::constructible_from< Ty, Args... >
	static Ty* alloc( const size_t _count, const std::source_location& _location, Args&&... _args )
	{
		const size_t size = get_size< Ty >( _count ) + sizeof( size_t );

		auto count_ptr = static_cast< size_t* >( Tracker::alloc( size, _location ) );
		    *count_ptr = _count;

		Ty* ptr = reinterpret_cast< Ty* >( count_ptr + 1 );

		for( size_t i = 0; i < _count; i++ )
			::new( ptr + i ) Ty( std::forward< Args >( _args )... );

		return ptr;

	} // alloc

	template< typename Ty, typename... Args >
	static Ty* alloc_no_tracker( const size_t _count, Args&&... _args )
	{
		const size_t size = get_size< Ty >( _count ) + sizeof( size_t );

		auto count_ptr = static_cast< size_t* >( Memory::alloc_fast( size ) );
		*count_ptr = _count;

		Ty* ptr = reinterpret_cast< Ty* >( count_ptr + 1 );

		for( size_t i = 0; i < _count; i++ )
			::new( ptr + i ) Ty( std::forward< Args >( _args )... );

		return ptr;

	} // alloc_no_tracker

	template< typename Ty >
	static void free( Ty* _block )
	{
		if( _block == nullptr )
			return;
		
		const auto count_ptr = reinterpret_cast< size_t* >( _block ) - 1;

		for( size_t i = 0; i < *count_ptr; i++ )
			_block[ i ].~Ty();

#if defined( SK_TRACKER_DISABLED )
		Memory::free_fast( count_ptr );
#else  // DEBUG
		Tracker::free( count_ptr );
#endif // !DEBUG

	} // free

	extern void free( void* _block, const std::source_location& = std::source_location::current() );

} // sk::Memory::

// TODO: Make macros reuse the SK_NEW macro in case of future changes.
#if !defined( SK_TRACKER_DISABLED )
/**
 * Default tracked alloc.
 * 
 * Arguments: Byte Size
 */
#define SK_ALLOC( Size ) sk::Memory::Tracker::alloc( Size )
/**
 * Default tracked alloc.
 * 
 * Arguments: Byte Size
 */
#define SK_REALLOC( Block, NewSize ) sk::Memory::Tracker::realloc( Block, NewSize )
/**
 * Default tracked new.
 * 
 * Arguments: Type, Count, Args...
 */
#define SK_NEW( Ty, Count, ... ) sk::Memory::alloc< Ty >( Count, std::source_location::current() __VA_OPT__(,) __VA_ARGS__ )
/**
 * Virtual array tracked new.
 *
 * Makes a pointer array with declared type.
 * 
 * Arguments: Type, Count.
 */
#define SK_VIRTUAL( Ty, Count ) sk::Memory::alloc< Ty*, Ty* >( Count, std::source_location::current(), nullptr )
/**
 * Single object tracked new.
 * 
 * Arguments: Type, Args...
 */
#define SK_SINGLE( Ty, ... ) SK_NEW( Ty, 1, __VA_ARGS__ )
/**
 * Single object without params tracked new.
 * 
 * Arguments: Type, Args...
 */
#define SK_SINGLE_EMPTY( Ty ) sk::Memory::alloc< Ty >( 1, std::source_location::current() )
/**
 * Tracked delete.
 * Functions like the usual delete.
 * 
 * NOTE: It does nullptr check for you.
 * 
 * Arguments: Address
 */
#define SK_DELETE( address ) sk::Memory::free( address )
/**
 * Tracked free.
 *
 * NOTE: It does nullptr check for you.
 *
 * Arguments: Address
 */
#define SK_FREE( address ) SK_DELETE( static_cast< void* >( address ) )
#else // !SK_TRACKER_DISABLED
/**
 * Default tracked alloc.
 * 
 * Arguments: Byte Size
 */
#define QW_ALLOC( Size ) sk::Memory::alloc_fast( Size )
/**
 * Default new.
 * 
 * Arguments: Type, Count, Args...
 */
#define QW_NEW( Ty, ... ) sk::Memory::alloc_no_tracker< Ty >( __VA_ARGS__ )
/**
 * Virtual array tracked new.
 *
 * Makes a pointer array with declared type.
 * 
 * Arguments: Type, Count.
 */
#define QW_VIRTUAL( Ty, Count ) sk::Memory::alloc_no_tracker< Ty* >( Count, nullptr )
/**
 * Single object tracked new.
 * 
 * Arguments: Type, Args...
 */
#define QW_SINGLE( Ty, ... ) sk::Memory::alloc_no_tracker< Ty >( 1, __VA_ARGS__ )
/**
 * Single object without params tracked new.
 * 
 * Arguments: Type, Args...
 */
#define QW_SINGLE_EMPTY( Ty ) sk::Memory::alloc_no_tracker< Ty >( 1 )
/**
 * Free.
 * 
 * Arguments: Address
 */
#define QW_FREE( address ) sk::Memory::free( address )


#endif // QW_TRACKER_DISABLED