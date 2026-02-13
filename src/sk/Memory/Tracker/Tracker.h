/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Macros/Enum_Builder.h>
#include <sk/Memory/Memory.h>
#include <sk/Misc/Singleton.h>

#include <mutex>
#include <source_location>

// #define SK_TRACKER_DISABLED

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
		extern bool   contains( void* _block );

		extern void   init    ( void );
		extern void   shutdown( void );

	} // Tracker::

	// TODO: Move the extern functions to a different include to not have them be depending on the memory tracker.
	// This will also allow the singletons to be tracked.
	extern void* alloc_fast  ( size_t _size, eAlignment _alignment = default_alignment );
	extern void  free_fast   ( void*  _block );
	extern void* realloc_fast( void* _block, size_t _size, eAlignment _alignment = default_alignment );

	template< class Ty >
	struct sAlignedCount
	{
		alignas( Math::max( alignof( Ty ), alignof( size_t ) ) ) size_t count;
	};

	template< typename Ty, typename... Args >
	requires std::constructible_from< Ty, Args... >
	static Ty* alloc( const size_t _count, const std::source_location& _location, Args&&... _args )
	{
		constexpr auto align = static_cast< eAlignment >( Math::max< size_t >( alignof( Ty ), default_alignment ) );
		const size_t size = get_size< Ty, align >( _count ) + sizeof( sAlignedCount< Ty > );

		auto count_ptr = static_cast< sAlignedCount< Ty >* >( Tracker::alloc( size, _location ) );
		count_ptr->count = _count;

		Ty* ptr = reinterpret_cast< Ty* >( count_ptr + 1 );

		// ::new( ptr ) Ty[ _count ]( std::forward< Args >( _args )... );

		for( size_t i = 0; i < _count; i++ )
			::new( ptr + i ) Ty( std::forward< Args >( _args )... );

		return ptr;

	} // alloc

	template< typename Ty, typename... Args >
	static Ty* alloc_no_tracker( const size_t _count, Args&&... _args )
	{
		const size_t size = get_size< Ty >( _count ) + sizeof( sAlignedCount< Ty > );

		auto count_ptr = static_cast< sAlignedCount< Ty >* >( Memory::alloc_fast( size ) );
		count_ptr->count = _count;

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

		const auto count_ptr = reinterpret_cast< sAlignedCount< Ty >* >( _block ) - 1;

#if !defined( SK_TRACKER_DISABLED )
		SK_BREAK_RET_IF( sk::Severity::kEngine, !Tracker::contains( count_ptr ),
			"Error: Cannot free pointer not allocated by tracker." )
#endif // !SK_TRACKER_DISABLED

		for( size_t i = 0; i < count_ptr->count; i++ )
			_block[ i ].~Ty();

#if defined( SK_TRACKER_DISABLED )
		Memory::free_fast( count_ptr );
#else  // SK_TRACKER_DISABLED
		Tracker::free( count_ptr );
#endif // !SK_TRACKER_DISABLED

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
#define SK_ALLOC( Size ) sk::Memory::alloc_fast( Size )
/**
 * Default tracked alloc.
 *
 * Arguments: Byte Size
 */
#define SK_REALLOC( Block, NewSize ) sk::Memory::realloc_fast( Block, NewSize )
/**
 * Default new.
 * 
 * Arguments: Type, Count, Args...
 */
#define SK_NEW( Ty, ... ) sk::Memory::alloc_no_tracker< Ty >( __VA_ARGS__ )
/**
 * Virtual array tracked new.
 *
 * Makes a pointer array with declared type.
 * 
 * Arguments: Type, Count.
 */
#define SK_VIRTUAL( Ty, Count ) sk::Memory::alloc_no_tracker< Ty* >( Count, nullptr )
/**
 * Single object tracked new.
 * 
 * Arguments: Type, Args...
 */
#define SK_SINGLE( Ty, ... ) sk::Memory::alloc_no_tracker< Ty >( 1, __VA_ARGS__ )
/**
 * Single object without params tracked new.
 * 
 * Arguments: Type, Args...
 */
#define SK_SINGLE_EMPTY( Ty ) sk::Memory::alloc_no_tracker< Ty >( 1 )
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
#endif // QW_TRACKER_DISABLED