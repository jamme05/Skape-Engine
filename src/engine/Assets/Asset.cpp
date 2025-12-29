/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset.h"

#include <typeindex>
#include <Assets/Management/Asset_Manager.h>
#include <Assets/Management/Asset_Job_Manager.h>

sk::cAsset_Meta::cAsset_Meta( const std::string_view _name, const type_info_t _asset_type )
: m_name_{ _name }
, m_asset_type_{ _asset_type }
{
    SK_ERR_IF( m_asset_type_->type != sType_Info::eType::kClass,
        "Error: Asset HAS to be a class type." )
}

void sk::cAsset_Meta::Save()
{
    static constexpr cStringID test = std::string_view{ "Hello" };
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

auto sk::cAsset_Meta::GetType() const -> type_info_t
{
    return m_asset_type_;
}

auto sk::cAsset_Meta::GetClass() const -> const iRuntimeClass*
{
    return m_asset_type_->as_class_info()->runtime_class;
}

auto sk::cAsset_Meta::GetHash() const -> type_hash
{
    return m_asset_type_->hash;
}

size_t sk::cAsset_Meta::AddListener( const dispatcher_t::event_t& _listener )
{
    dispatch_if_loaded( _listener.function, nullptr, true );
			
    return m_dispatcher_.add_listener( _listener );
}

size_t sk::cAsset_Meta::AddListener( const dispatcher_t::weak_event_t& _listener )
{
    dispatch_if_loaded( _listener.function, nullptr, true );

    return m_dispatcher_.add_listener( Event::sEvent{ _listener } );
}

void sk::cAsset_Meta::RemoveListener( const size_t _listener_id )
{
    m_dispatcher_.remove_listener_by_id( _listener_id );
}

void sk::cAsset_Meta::RemoveListener( const dispatcher_t::event_t& _listener )
{
    m_dispatcher_.remove_listener( _listener );
}

void sk::cAsset_Meta::RemoveListener( const dispatcher_t::weak_event_t& _listener )
{
    m_dispatcher_.remove_listener( _listener );
}

void sk::cAsset_Meta::addReferrer( void* _source, const cWeak_Ptr< iClass >& _referrer )
{
    ++m_asset_refs_;
    
    // Detailed ref counting is only done in debug to save performance
    // TODO: Add a specific define for if the asset references will be tracked.
#ifdef DEBUG
    m_referrers_.emplace( _referrer, _source );
    cAsset_Manager::get().addPathReferrer( m_path_.hash(), _source );
#endif // DEBUG
    
    if( IsLoaded() )
        return;

    push_load_task( true, _source );
}

void sk::cAsset_Meta::removeReferrer( const void* _source, const cWeak_Ptr< iClass >& _referrer )
{
#ifdef DEBUG
    // TODO: Think of a way to make this faster.
    auto [ fst, lst ] = m_referrers_.equal_range( _referrer );
    for( auto it = fst; it != lst; ++it )
    {
        if( it->second != _source )
            continue;
        
        m_referrers_.erase( it );
        break;
    }
    
    cAsset_Manager::get().addPathReferrer( m_path_.hash(), _source );
#endif // DEBUG

    if( --m_asset_refs_ != 0 )
        return;

    push_load_task( false, _source );
}

void sk::cAsset_Meta::push_load_task( const bool _load, const void* _source ) const
{
    using namespace Assets;
    
    auto& asset_manager = cAsset_Manager::get();

    const auto loader = asset_manager.GetFileLoader( m_ext_.hash() );

    SK_BREAK_RET_IF( sk::Severity::kConstEngine, loader == nullptr,
        "Error no loader for asset." )

    // TODO: Add a cache for the affected assets.
    const auto affected = asset_manager.getAssetsByPath( m_path_.hash() );
    
    Jobs::sTask task;
    task.type = _load ? Jobs::eJobType::kLoad : Jobs::eJobType::kUnload;
    task.data = ::new( Memory::alloc_fast( sizeof( Jobs::sAssetTask ) ) ) Jobs::sAssetTask{
        .path   = m_absolute_path_,
        .affected_assets = affected,
        .loader = loader,
        .source = _source,
    };
    
    Jobs::cAsset_Job_Manager::get().push_task( task );
}

void sk::cAsset_Meta::dispatch_if_loaded( const dispatcher_t::listener_t& _listener, const void* _source, bool _is_loading )
{
    if( !IsLoaded() )
        return;

    const Assets::Jobs::sTask task
    {
        .type = Assets::Jobs::eJobType::kPushEvent,
        .data = ::new( Memory::alloc_fast( sizeof( Assets::Jobs::sListenerTask ) ) ) Assets::Jobs::sListenerTask{
            .meta   = get_shared(),
            .event  = _listener,
            .source = _source,
        },
    };

    Assets::Jobs::cAsset_Job_Manager::get().push_task( task );
}

void sk::cAsset_Meta::setPath( std::filesystem::path _path )
{
    m_absolute_path_ = _path;
    m_ext_  = _path.extension().string().substr( 1 );
    m_path_ = _path.replace_extension().string();
}

