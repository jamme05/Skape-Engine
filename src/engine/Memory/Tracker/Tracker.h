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

#include "Macros/Enum_Builder.h"

namespace sk::Memory
{
	namespace Tracker
	{
		// Decides if the memory tracker should keep track of the entire history and not just the latest modification.
		constexpr bool kSaveMemoryHistory = true;

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

	} // Tracker::

	class cTracker : public cSingleton< cTracker >
	{
	public:
		// 16-bit for align.
		MAKE_UNREFLECTED_ENUM( ENUMCLASS( eAction, uint16_t ),
			E( kAllocate ),
			E( kReallocate ),
			E( kFree )
		);

		struct sHistory_action
		{
			sHistory_action( const std::source_location& _location, const eAction _action )
			: file_name( _location.file_name() )
			, function( _location.function_name() )
			, line( static_cast< uint32_t >( _location.line() ) )
			, column( static_cast< uint32_t >( _location.column() ) )
			, action( _action )
			{} // sFile_info
	
			const char* file_name;
			const char* function;
			uint32_t    line;
			uint16_t    column;
			eAction     action;
		};

		struct sTracker_entry
		{
			sHistory_action* history;
			sHistory_action* last;
			// N allocation. Aka incrementing by one each allocation but can't find a good word for it.
			uint32_t    allocation;
			// The alignment used for the allocation
			uint16_t    alignment;
			// Number of changes the entry has had. The will probably not reach more than 255 allocations.
			uint8_t    changes;
			uint8_t     flags;
			// Non-aligned size of the allocation
			size_t      size;
		};
		constexpr static size_t aligned_entry_size = get_size< sTracker_entry >();

		struct sStatistics
		{
			size_t memory_allocated_exc_head = 0u;
			size_t memory_allocated_inc_head = 0u;
			size_t memory_freed_exc_head     = 0u;
			size_t memory_freed_inc_head     = 0u;
			size_t allocation_count          = 0u;
			size_t free_count                = 0u;
		};

		typedef std::unordered_set< void* > block_set_t;
		typedef std::vector< sHistory_action* >  history_t;  

		void* alloc  ( const size_t _size, const std::source_location& _location = std::source_location::current() );
		void  free   ( void* _block, const std::source_location& _location = std::source_location::current() );
		void* realloc( void* _block, const size_t _size, const std::source_location& _location = std::source_location::current() );

		 cTracker( void );
		~cTracker( void );

		static void* Alloc( const size_t _size, const auto& _location = std::source_location::current() ){ return get().alloc( _size, _location ); }
		static void  Free ( void*  _block ){ get().free( _block ); }

		static size_t GetAvailableHeapSpace( void );

		static size_t m_memory_usage;

private:
		void add_history( sTracker_entry& _entry, const sHistory_action& _action );

		// NOTE: The save history is placed in front
		block_set_t m_block_set;
		history_t   m_history;
		sStatistics m_statistics;
		std::mutex  m_mtx;

	}; // cTracker

	extern void* alloc_fast( size_t _size );
	extern void  free_fast ( void*  _block );
	extern void* realloc_fast( void* _block, size_t _size );

	template< typename Ty, typename... Args >
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
		const auto count_ptr = reinterpret_cast< size_t* >( _block ) - 1;

		for( size_t i = 0; i < *count_ptr; i++ )
			_block[ i ].~Ty();

#if defined( QW_TRACKER_DISABLED )
		Memory::free_fast( count_ptr );
#else  // DEBUG
		Tracker::free( count_ptr );
#endif // !DEBUG

	} // free

	extern void free( void* _block );

} // sk::Memory::

#if !defined( QW_TRACKER_DISABLED )
/**
 * Default tracked alloc.
 * 
 * Arguments: Byte Size
 */
#define QW_ALLOC( Size ) sk::Memory::Tracker::alloc( Size )
/**
 * Default tracked new.
 * 
 * Arguments: Type, Count, Args...
 */
#define QW_NEW( Ty, Count, ... ) sk::Memory::alloc< Ty >( Count, std::source_location::current() __VA_OPT__(,) __VA_ARGS__ )
/**
 * Virtual array tracked new.
 *
 * Makes a pointer array with declared type.
 * 
 * Arguments: Type, Count.
 */
#define QW_VIRTUAL( Ty, Count ) sk::Memory::alloc< Ty*, Ty* >( Count, std::source_location::current(), nullptr )
/**
 * Single object tracked new.
 * 
 * Arguments: Type, Args...
 */
#define QW_SINGLE( Ty, ... ) sk::Memory::alloc< Ty >( 1, std::source_location::current(), __VA_ARGS__ )
/**
 * Single object without params tracked new.
 * 
 * Arguments: Type, Args...
 */
#define QW_SINGLE_EMPTY( Ty ) sk::Memory::alloc< Ty >( 1, std::source_location::current() )
/**
 * Tracked free.
 * 
 * Arguments: Address
 */
#define QW_FREE( address ) sk::Memory::free( address )
#else // !QW_TRACKER_DISABLED
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