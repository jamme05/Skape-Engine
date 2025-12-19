/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset.h"

#include <Assets/Management/Asset_Manager.h>

#include "Management/Asset_Job_Manager.h"

void sk::cAsset_Meta::Save()
{
    // TODO: Saving logic and actual metadata saving.
}

bool sk::cAsset_Meta::IsLoading() const
{
    return m_flags_ & kLoading;
}

bool sk::cAsset_Meta::IsLoaded() const
{
    return m_flags_ & kLoaded;
}

bool sk::cAsset_Meta::HasMetadata() const
{
    return m_flags_ & kMetadata;
}

auto sk::cAsset_Meta::GetAsset() const -> cAsset*
{
    return m_asset_;
}

auto sk::cAsset_Meta::GetFlags() const
{
    return m_flags_.load();
}

size_t sk::cAsset_Meta::AddListener( const dispatcher_t::event_t& _listener )
{
    dispatch_if_loaded( _listener.function );
			
    return m_dispatcher_.add_listener( _listener );
}

size_t sk::cAsset_Meta::AddListener( const dispatcher_t::weak_event_t& _listener )
{
    dispatch_if_loaded( _listener.function );

    return m_dispatcher_.add_listener( _listener );
}

void sk::cAsset_Meta::RemoveListener( const size_t _listener_id )
{
    m_dispatcher_.remove_listener( _listener_id );
}

void sk::cAsset_Meta::RemoveListener( const dispatcher_t::event_t& _listener )
{
    m_dispatcher_.remove_listener( _listener );
}

void sk::cAsset_Meta::RemoveListener( const dispatcher_t::weak_event_t& _listener )
{
    m_dispatcher_.remove_listener( _listener );
}

void sk::cAsset_Meta::dispatch_if_loaded( const dispatcher_t::listener_t& _listener )
{
    if( IsLoaded() == 0 )
        return;

    const Assets::Jobs::sTask task
    {
        .type = Assets::Jobs::eJobType::kPushEvent,
        .data = reinterpret_cast< std::byte* >( new Assets::Jobs::sListenerTask{
            .partial = get_shared(), .event = _listener
        } )
    };

    Assets::Jobs::cAsset_Job_Manager::get().push_task( task );
}

void sk::cAsset_Meta::setPath( const std::filesystem::path& _path )
{
    m_path_ = _path.string();
}

