/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include <Memory/Tracker/Tracker.h>
#include <Windows.h>

namespace sk::Memory
{
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
        }
        void* alloc( const size_t _size, const std::source_location& _location )
        {
            if( const auto tracker = cTracker::getPtr() )
                return tracker->alloc( _size, _location );
            return malloc( _size );
        } // alloc
        void* realloc( void* _ptr, const size_t _size, const std::source_location& _location )
        {
            if( const auto tracker = cTracker::getPtr() )
                return tracker->realloc( _ptr, _size, _location );
            return realloc( _ptr, _size, _location );
        } // realloc
        void free( void* _block, const std::source_location& _location )
        {
            if( const auto tracker = cTracker::getPtr() )
                return tracker->free( _block, _location );
            return free( _block );
        } // free
        size_t max_heap_size( void )
        {
            return max_allowed_memory_size;
        } // max_heap_size
    } // Tracker::

    void* cTracker::alloc( const size_t _size, const std::source_location& _location )
    {
        const auto size   = get_aligned( _size );
        const auto t_size = size + aligned_entry_size;

        const auto entry = static_cast< sTracker_entry* >( ::malloc( t_size ) );

        entry->changes       = 0;
        entry->allocation    = static_cast< uint32_t >( m_statistics.allocation_count++ );
        entry->changes       = 0;
        entry->flags         = Tracker::eEntryFlags::kNone;

        entry->alignment     = static_cast< uint16_t >( size - _size );
        entry->size          = _size;

        m_memory_usage += t_size;

        m_mtx.lock();
        if constexpr( Tracker::kSaveMemoryHistory )
        {
            add_history( *entry, { _location, eAction::kAllocate } );
        }
        m_block_set.insert( entry );
        m_mtx.unlock();

        return static_cast< void* >( entry + 1 );
    } // alloc

    void cTracker::free( void* _block, const std::source_location& _location )
    {
        const auto entry = static_cast< sTracker_entry* >( _block ) - 1;

        ::free( entry );

        m_mtx.lock();
        if constexpr( Tracker::kSaveMemoryHistory )
        {
            add_history( *entry, { _location, eAction::kFree } );
        }
        m_block_set.erase( entry );
        m_mtx.unlock();
    } // free

    void* cTracker::realloc( void* _block, const size_t _size, const std::source_location& _location )
    {
    } // realloc

    cTracker::cTracker( void )
    {
        MEMORYSTATUSEX mem_status;
        mem_status.dwLength = sizeof( mem_status );

        GlobalMemoryStatusEx( &mem_status );

        Tracker::total_memory_size       = static_cast< size_t >( mem_status.ullTotalPhys );
        Tracker::max_allowed_memory_size = static_cast< size_t >( static_cast< float >( mem_status.ullTotalPhys ) * Tracker::max_memory_usage_percentage );
    } // cTracker

    cTracker::~cTracker( void )
    {
        for( auto& entry : m_block_set )
        {
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
        // If getting allocated it will create the history
        if( _action.action == eAction::kAllocate )
        {
            constexpr auto history_action_size = get_size< sHistory_action, kHalfAlign >();
            _entry.last  = _entry.history = m_history.emplace_back( static_cast< sHistory_action* >( ::malloc( history_action_size ) ) );
            *_entry.last = _action;
        }
        else
        {
            // TODO: Add logic for when reallocated or freed.
        }
    } // add_history

    void* alloc_fast( size_t _size )
    {
        _size += sizeof( size_t );
        const auto size = static_cast< size_t* >( ::malloc( make_aligned( _size ) ) );
        cTracker::m_memory_usage += *size;
        return size + 1;
    } // alloc_fast

    void free_fast( void* _block )
    {
        const auto size = static_cast< size_t* >( _block ) - 1;
        cTracker::m_memory_usage -= *size;
        return ::free( size );
    } // free_fast

    void* realloc_fast( void* _block, size_t _size )
    {
        _size += sizeof( size_t );
        const auto size = static_cast< size_t* >( _block ) - 1;
        cTracker::m_memory_usage -= *size;
        const auto n_size = ::realloc( size, make_aligned( _size ) );
        cTracker::m_memory_usage += n_size;
        return n_size + 1;
    } // realloc_fast
} // sk::Memory::Tracker
