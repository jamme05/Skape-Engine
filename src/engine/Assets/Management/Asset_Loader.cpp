/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Loader.h"

sk::cAsset_Worker::cAsset_Worker()
{
    
}

void sk::cAsset_Worker::worker( cAsset_Worker* _loader )
{
    auto& self = *_loader;

    std::list< sTask > tasks;
    while( self.m_working_ )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

        if( self.m_queue_size_.load() == 0 )
            continue;

        self.m_queue_locked_.wait( true );
        self.m_queue_locked_.store( true );

        tasks.swap( self.m_queue_ );

        self.m_queue_locked_.store( false );
        self.m_queue_locked_.notify_one();

        for( auto& task : tasks )
            do_work( self, task );
        
        tasks.clear();
    }
}

void sk::cAsset_Worker::do_work( cAsset_Worker& _self, sTask& _task )
{
    switch( _task.type )
    {
    case eJobType::kLoad:
    {
        auto partial_asset = _task.info.asset_task.partial.Cast< cAssetMeta >();
        auto path_to_load = partial_asset->GetPath();

        const auto extension_hash = str_hash( std::filesystem::path{ path_to_load.string() }.extension() );

        const auto callback_pair = _self.m_extension_map_.find( extension_hash );

        callback_pair->second( *partial_asset, cAsset_Manager::eLoadTask::kLoadAsset );
    }
    break;
    case eJobType::kUnload:
    {
        auto partial_asset = _task.info.asset_task.partial.Cast< cAssetMeta >();

        SK_FREE( partial_asset->m_asset_ );
        partial_asset->m_asset_ = nullptr;
    }
    break;
    case eJobType::kRefresh:
    {
        
    }
    break;
    case eJobType::kPushEvent:
    {
        
    }
    break;
    case eJobType::kUpdateExtensions:
    {
        _self.m_extension_map_.swap( _self.m_new_extension_map_ );
    }
    break;
case eJobType::kPushEventWeak:
    break;
    }
}


void sk::cAsset_Worker::AddAssetTask( const eJobType _type, const cShared_ptr< cAssetMeta >& _asset )
{
    auto new_task = sTask{};
    new_task.type = _type;
    
    new_task.info.asset_task.partial = _asset;

    push_task( std::move( new_task ) );
}

void sk::cAsset_Worker::AddListenerEvent( const partial_t& _asset, const listener_t& _listener )
{
    auto new_task = sTask{};
    new_task.type = eJobType::kPushEvent;
    
    new_task.info.listener_task.partial          = _asset;
    new_task.info.listener_task.event.void_event = _listener;

    push_task( std::move( new_task ) );
}

void sk::cAsset_Worker::AddListenerEvent( const partial_t& _asset, const bool_listener_t& _listener )
{
    auto new_task = sTask{};
    new_task.type = eJobType::kPushEventWeak;
    
    new_task.info.listener_task.partial          = _asset;
    new_task.info.listener_task.event.void_event = _listener;

    push_task( std::move( new_task ) );
}

void sk::cAsset_Worker::UpdateExtensions( const unordered_map< str_hash, cAsset_Manager::load_file_func_t >& _new_map )
{
    auto new_task = sTask{};
    new_task.type = eJobType::kUpdateExtensions;

    m_new_extension_map_ = _new_map;

    push_task( std::move( new_task ) );
}

void sk::cAsset_Worker::push_task( sTask&& _task )
{
    m_queue_locked_.wait( true );
    
    m_queue_locked_.store( true );
    m_queue_.emplace_back( std::move( _task ) );
    
    m_queue_locked_.store( false );
    m_queue_locked_.notify_one();
}
