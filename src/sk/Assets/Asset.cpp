/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset.h"

#include <sk/Assets/Management/Asset_Job_Manager.h>
#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Seralization/SerializedObject.h>

using namespace sk;

cAsset_Meta::cAsset_Meta( const std::string_view _name, const type_info_t _asset_type )
: m_name_{ _name }
, m_asset_type_{ _asset_type }
{
    SK_ERR_IF( m_asset_type_->type != sType_Info::eType::kClass,
        "Error: Asset HAS to be a class type." )
}

void cAsset_Meta::Save()
{
    // TODO: Saving logic and actual metadata saving.
}

bool cAsset_Meta::IsLoading() const
{
    return ( m_flags_ & kLoading ) != 0;
}

bool cAsset_Meta::IsLoaded() const
{
    return ( m_flags_ & kLoaded ) != 0;
}

bool cAsset_Meta::IsLoadingOrLoaded() const
{
    return IsLoading() || IsLoaded();
}

bool cAsset_Meta::HasMetadata() const
{
    return m_flags_ & kMetadata;
}

auto cAsset_Meta::GetUUID() const -> cUUID
{
    return m_uuid_;
}

auto cAsset_Meta::GetAsset() const -> cAsset*
{
    return m_asset_;
}

auto cAsset_Meta::GetFlags() const
{
    return m_flags_.load();
}

auto cAsset_Meta::GetType() const -> type_info_t
{
    return m_asset_type_;
}

auto cAsset_Meta::GetClass() const -> const iRuntimeClass*
{
    return m_asset_type_->as_class_info()->runtime_class;
}

auto cAsset_Meta::GetHash() const -> type_hash
{
    return m_asset_type_->hash;
}

void cAsset_Meta::Reload()
{
    auto& manager = cAsset_Manager::get();
    Assets::Jobs::sTask task;
    task.type = Assets::Jobs::eJobType::kRefresh;

    task.data = ::new( Memory::alloc_fast( sizeof( Assets::Jobs::sAssetTask ) ) ) Assets::Jobs::sAssetTask{
        .path    = m_absolute_path_.view(), 
        .affected_assets = manager.GetAssetsByPathHash( m_absolute_path_ ),
        .loader = manager.GetFileLoader( m_ext_ ),
    };
    
    Assets::Jobs::cAsset_Job_Manager::get().push_task( task );
}

size_t cAsset_Meta::AddListener( const dispatcher_t::event_t& _listener )
{
    dispatch_if_loaded( _listener.function, nullptr, true );

    std::println( "Adding listener to {}", m_absolute_path_.view() );
			
    return m_dispatcher_.add_listener( _listener );
}

size_t cAsset_Meta::AddListener( const dispatcher_t::weak_event_t& _listener )
{
    dispatch_if_loaded( _listener.function, nullptr, true );

    return m_dispatcher_.add_listener( Event::sEvent{ _listener } );
}

void cAsset_Meta::RemoveListener( const size_t _listener_id )
{
    m_dispatcher_.remove_listener_by_id( _listener_id );
}

void cAsset_Meta::RemoveListener( const dispatcher_t::event_t& _listener )
{
    m_dispatcher_.remove_listener( _listener );
}

void cAsset_Meta::RemoveListener( const dispatcher_t::weak_event_t& _listener )
{
    m_dispatcher_.remove_listener( _listener );
}

void cAsset_Meta::LockAsset()
{
    ++m_lock_refs_;
}

void cAsset_Meta::UnlockAsset()
{
    if( --m_lock_refs_ != 0 || m_asset_refs_.load() != 0 )
        return;
    
    push_load_task( false, nullptr );
}

void cAsset_Meta::addReferrer( void* _source, const cWeak_Ptr< iClass >& _referrer )
{
    ++m_asset_refs_;
    
    // Detailed ref counting is only done in debug to save performance
    // TODO: Add a specific define for if the asset references will be tracked.
#ifdef DEBUG
    m_referrers_.emplace( _referrer, _source );
#endif // DEBUG
    cAsset_Manager::get().addPathReferrer( m_path_.hash(), _source );
    
    if( IsLoadingOrLoaded() )
        return;

    std::println( "Requesting load for asset with path {}", m_absolute_path_.view() );
    
    // We mark it early.
    m_flags_ |= kLoading;

    push_load_task( true, _source );
}

void cAsset_Meta::removeReferrer( const void* _source, const cWeak_Ptr< iClass >& _referrer )
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
#endif // DEBUG

    const auto can_remove = cAsset_Manager::get().removePathReferrer( m_path_.hash(), _source );
    
    if( --m_asset_refs_ != 0 )
        return;
    
    if( m_lock_refs_.load() > 0 )
        return;

    if( m_flags_ & kManualCreation )
        setAsset( nullptr );
    else if( can_remove )
        push_load_task( false, _source );
}

void cAsset_Meta::push_load_task( const bool _load, const void* _source ) const
{
    using namespace Assets;
    
    auto& asset_manager = cAsset_Manager::get();

    const auto loader = asset_manager.GetFileLoader( m_ext_.hash() );

    SK_BREAK_RET_IF( sk::Severity::kConstEngine, loader == nullptr,
        "Error no loader for asset." )

    // TODO: Add a cache for the affected assets.
    const auto affected = asset_manager.GetAssetsByPathHash( m_absolute_path_ );
    
    for( auto& asset : affected )
        asset->m_flags_ |= kLoading;
    
    Jobs::sTask task;
    task.type = _load ? Jobs::eJobType::kLoad : Jobs::eJobType::kUnload;
    task.data = ::new( Memory::alloc_fast( sizeof( Jobs::sAssetTask ) ) ) Jobs::sAssetTask{
        .path   = m_absolute_path_.view(),
        .affected_assets = affected,
        .loader = loader,
    };
    
    Jobs::cAsset_Job_Manager::get().push_task( task );
}

void cAsset_Meta::dispatch_if_loaded( const dispatcher_t::listener_t& _listener, const void* _source, bool _is_loading )
{
    if( !IsLoaded() )
        return;

    std::println( "Asset with path {} already loaded, subscribing to single event", m_name_.view() );

    const Assets::Jobs::sTask task
    {
        .type = Assets::Jobs::eJobType::kPushEvent,
        .data = ::new( Memory::alloc_fast( sizeof( Assets::Jobs::sListenerTask ) ) ) Assets::Jobs::sListenerTask{
            .meta   = get_shared(),
            .event  = _listener,
        },
    };

    Assets::Jobs::cAsset_Job_Manager::get().push_task( task );
}

cShared_ptr< cSerializedObject > cAsset::Serialize()
{
    return m_metadata_->GetUUID().Serialize();
}

void cAsset_Meta::setPath( std::filesystem::path _path )
{
    _path.make_preferred();
    m_absolute_path_ = _path.string();
    m_ext_  = _path.extension().string().substr( 1 );
    m_path_ = _path.replace_extension().string();
}

void cAsset_Meta::setAsset( cAsset* _asset )
{
    if( m_asset_ != nullptr )
    {
        // There seems to be some sort of race condition
        // So this ensures that the asset gets unreferenced before this thread gets stuck waiting for the memory tracker.
        const auto tmp = m_asset_;
        m_asset_ = nullptr;
        SK_DELETE( tmp );
    }
    
    m_asset_ = _asset;
    
    if( m_asset_ != nullptr )
    {
        m_asset_->m_metadata_ = get_weak();
        m_flags_ |=  kLoaded;
        m_flags_ &= ~kLoading;
    }
    else
        m_flags_ &= ~kLoaded & ~kLoading;
}

