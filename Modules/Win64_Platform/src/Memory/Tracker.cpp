/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include <Memory/Tracker/Tracker.h>
#include <Windows.h>

namespace sk::Memory
{
    class cTracker : public cSingleton< cTracker >
	{
	public:
		// 16-bit for align.
		MAKE_UNREFLECTED_ENUM( ENUMCLASS( eAction, uint16_t ),
			E( kAllocate, "Allocation" ),
			E( kReallocate, "Reallocation" ),
			E( kFree )
		);

		struct sHistory_action
		{
			sHistory_action( const std::source_location& _location, const eAction _action, const size_t _size )
			: file_name( _location.file_name() )
			, function( _location.function_name() )
			, line( static_cast< uint32_t >( _location.line() ) )
			, column( static_cast< uint32_t >( _location.column() ) )
			, action( _action )
			, size( _size )
			{} // sFile_info
	
			const char* file_name;
			const char* function;
			uint32_t    line;
			uint16_t    column;
			eAction     action;
			size_t      size;
		};

		struct sTracker_entry
		{
			sHistory_action** history;
			sHistory_action*  last;
			// N allocation. Aka incrementing by one each allocation but can't find a good word for it.
			uint32_t    allocation;
			// The size of alignment and entry. size + extra = total_size
			uint16_t    extra;
			// Number of changes the entry has had. The will probably not reach more than 255 allocations.
			uint8_t	    history_length;
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
			size_t reallocation_count        = 0u;
		};

		typedef std::unordered_set< sTracker_entry* > block_set_t;
		typedef std::vector< sHistory_action* >  history_t;  

		void* alloc  ( const size_t _size, const std::source_location& _location = std::source_location::current() );
		void  free   ( void* _block, const std::source_location& _location = std::source_location::current() );
		void* realloc( void* _block, const size_t _size, const std::source_location& _location = std::source_location::current() );

		 cTracker( void );
		~cTracker( void ) override;

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
	size_t cTracker::m_memory_usage = 0;

    namespace Tracker
    {
        enum eEntryFlags : uint8_t
        {
            kNone    = 0x00,
            // In case the entry has memory history.
            kHistory = 0x01,
        };

        namespace
        {
            size_t total_memory_size       = 0;
            size_t max_allowed_memory_size = 0;
            // Uses percentage of 
            constexpr float  max_memory_usage_percentage = 0.2f;
        } // ::

        void* alloc( const size_t _size, const std::source_location& _location )
        {
            if( const auto tracker = cTracker::getPtr() )
                return tracker->alloc( _size, _location );
            return alloc_fast( _size );
        } // alloc

        void* realloc( void* _ptr, const size_t _size, const std::source_location& _location )
        {
            if( const auto tracker = cTracker::getPtr() )
                return tracker->realloc( _ptr, _size, _location );
            return realloc_fast( _ptr, _size );
        } // realloc

        void free( void* _block, const std::source_location& _location )
        {
            if( const auto tracker = cTracker::getPtr() )
                return tracker->free( _block, _location );
            return free_fast( _block );
        } // free

        size_t max_heap_size( void )
        {
            return max_allowed_memory_size;
        } // max_heap_size

        void init( void )
        {
        	// Windows Only. In case of copying this tracker to a different platform, make sure to remove this.
        	MEMORYSTATUSEX mem_status;
        	mem_status.dwLength = sizeof( mem_status );

        	GlobalMemoryStatusEx( &mem_status );

        	total_memory_size       = static_cast< size_t >( mem_status.ullTotalPhys );
        	max_allowed_memory_size = static_cast< size_t >( static_cast< float >( mem_status.ullTotalPhys ) * max_memory_usage_percentage );

        	// But remember to start the Tracker!
        	cTracker::init();
        } // init

        void shutdown( void )
        {
        	cTracker::shutdown();
        } // shutdown

    } // Tracker::

    void* cTracker::alloc( const size_t _size, const std::source_location& _location )
    {
    	if( _size == 0 )
    		return nullptr;

        const auto size   = get_aligned( _size );
    	// Total size to be allocated.
        const auto t_size = size + aligned_entry_size;

    	// Add statistics
    	m_statistics.memory_allocated_exc_head += _size;
    	m_statistics.memory_allocated_inc_head += t_size;
    	m_memory_usage += t_size;

        const auto entry = static_cast< sTracker_entry* >( ::malloc( t_size ) );

    	// Make sure all info is set as default.
        entry->allocation = static_cast< uint32_t >( m_statistics.allocation_count++ );
        entry->flags      = Tracker::eEntryFlags::kNone;

    	entry->history    = nullptr;
        entry->size       = _size;
        entry->extra      = static_cast< uint16_t >( t_size - _size );

        m_memory_usage += t_size;

    	// Remember to lock whilst working with containers.
        m_mtx.lock();
        if constexpr( Tracker::kSaveMemoryHistory )
            add_history( *entry, { _location, eAction::kAllocate, _size } );
    	else
    		entry->last = static_cast< sHistory_action* >( alloc_fast( t_size, kB32Align ) );

        m_block_set.insert( entry );
        m_mtx.unlock();

    	// Remember to return the segment to the right of the entry ( which will be the working location )
        return static_cast< void* >( entry + 1 );
    } // alloc

    void cTracker::free( void* _block, const std::source_location& _location )
    {
    	// Get the entry.
        const auto entry = static_cast< sTracker_entry* >( _block ) - 1;

    	const auto total_size = entry->size + static_cast< size_t >( entry->extra );
    	++m_statistics.free_count;
    	m_statistics.memory_freed_exc_head += entry->size;
    	m_statistics.memory_freed_inc_head += total_size;
    	m_memory_usage -= total_size;

        m_mtx.lock();
        if constexpr( Tracker::kSaveMemoryHistory )
            add_history( *entry, { _location, eAction::kFree, 0 } );
    	else
    		free_fast( entry->last );

        m_block_set.erase( entry );
        m_mtx.unlock();

        ::free( entry );
    } // free

    void* cTracker::realloc( void* _block, const size_t _size, const std::source_location& _location )
    {
    	// Do a normal alloc in case no block provided.
    	if( _block == nullptr )
    		return alloc( _size, _location );

    	// Get the entry.
    	const auto prev_entry = static_cast< sTracker_entry* >( _block ) - 1;

    	const auto total_prev_size = prev_entry->size + static_cast< size_t >( prev_entry->extra );
    	m_statistics.memory_freed_exc_head += prev_entry->size;
    	m_statistics.memory_freed_inc_head += total_prev_size;
    	m_memory_usage -= total_prev_size;

    	const auto size   = get_aligned( _size );
    	const auto t_size = size + aligned_entry_size;
    	const auto entry = static_cast< sTracker_entry* >( ::realloc( prev_entry, t_size ) );

    	++m_statistics.reallocation_count;
    	m_statistics.memory_allocated_exc_head += size;
    	m_statistics.memory_allocated_inc_head += t_size;
    	m_memory_usage += t_size;

    	entry->size  = _size;
    	entry->extra = static_cast< uint16_t >( t_size - _size );

    	m_mtx.lock();
    	if constexpr( Tracker::kSaveMemoryHistory )
    	{
    		add_history( *entry, { _location, eAction::kFree, _size } );
    	}
    	m_block_set.erase( prev_entry );
    	m_block_set.insert( entry );
    	m_mtx.unlock();

    	return static_cast< void* >( entry + 1 );
    } // realloc

    cTracker::cTracker( void )
    {
    	
    } // cTracker

    cTracker::~cTracker( void )
    {
    	// TODO: Print statistics.
        for( auto& entry : m_block_set )
        {
        	// TODO Printer for memory leaks.
			// https://en.cppreference.com/w/cpp/utility/format/spec.html
        	free_fast( entry->last );
            ::free( entry );
        }
        m_block_set.clear();

        for( const auto& history : m_history )
        {
            ::free( history );
        }
    } // ~cTracker

    size_t cTracker::GetAvailableHeapSpace( void )
    {
        return Tracker::max_allowed_memory_size - m_memory_usage;
    } // GetAvailableHeapSpace

    void cTracker::add_history( sTracker_entry& _entry, const sHistory_action& _action )
    {
    	// TODO: Something is broken with source location. Debug later
    	// Will do nothing if save memory history is disabled.
    	if constexpr( Tracker::kSaveMemoryHistory )
    	{
    		switch( _action.action )
    		{
    		case eAction::kAllocate:
    		{
	            constexpr auto history_action_size = get_size< sHistory_action, kB32Align >();
    			_entry.history        = &m_history.emplace_back( static_cast< sHistory_action* >( alloc_fast( history_action_size, kB32Align ) ) );
	            _entry.last           = *_entry.history;
    			_entry.history_length = 1;
    		}
    		break;
    		case eAction::kReallocate:
    		case eAction::kFree:
    		{
    			// TODO: Store the history to disk once freed.
    			const auto history_action_size = get_size< sHistory_action, kB32Align >( ++_entry.history_length );
    			*_entry.history = static_cast< sHistory_action* >( realloc_fast( *_entry.history, history_action_size, kB32Align ) );
    			_entry.last = *_entry.history + ( _entry.history_length - 1 );
    		}
    		break;
    		}

    		// Always finish with setting the last history to the current action.
	        *_entry.last = _action;
    	}
    } // add_history

    void* alloc_fast( size_t _size, const eAlignment _alignment )
    {
    	if( _size == 0 )
    		return nullptr;

        _size += sizeof( size_t );
        const auto size = static_cast< size_t* >( ::malloc( make_aligned( _size, _alignment ) ) );
        cTracker::m_memory_usage += *size = _size;
        return size + 1;
    } // alloc_fast

    void free_fast( void* _block )
    {
        const auto size = static_cast< size_t* >( _block ) - 1;
        cTracker::m_memory_usage -= *size;
        return ::free( size );
    } // free_fast

    void* realloc_fast( void* _block, size_t _size, const eAlignment _alignment )
    {
    	// Normal alloc in case no block is provided.
    	if( _block == nullptr )
    		return alloc_fast( _size, _alignment );

        _size += sizeof( size_t );
        const auto size = static_cast< size_t* >( _block ) - 1;
        cTracker::m_memory_usage -= *size;
        const auto n_size = static_cast< size_t* >( ::realloc( size, make_aligned( _size, _alignment ) ) );
        cTracker::m_memory_usage += *n_size = _size;
        return n_size + 1;
    } // realloc_fast

    void free( void* _block, const std::source_location& _location )
    {
    	Tracker::free( _block, _location );
    } // free

} // sk::Memory::
