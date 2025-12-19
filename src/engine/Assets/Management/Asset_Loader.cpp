/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Loader.h"

#include "Asset_Job_Manager.h"

namespace
{
    sk::Assets::Jobs::cAsset_Job_Manager* manager = nullptr;
} // ::

sk::Assets::Jobs::cAsset_Worker::cAsset_Worker()
{
    if( manager == nullptr )
        manager = cAsset_Job_Manager::getPtr();
}

void sk::Assets::Jobs::cAsset_Worker::worker( const cAsset_Worker* _loader )
{
    auto& self = *_loader;

    while( self.m_working_ )
    {
        if( auto task = manager->WaitForTask(); task.type != eJobType::kNone )
            do_work( task );
    }
}

void sk::Assets::Jobs::cAsset_Worker::do_work( const sTask& _work )
{
    switch( _work.type ) {
    case eJobType::kNone: break;
    case eJobType::kLoad:
        load_asset( *reinterpret_cast< sAssetTask* >( _work.data ), false );
        break;
    case eJobType::kUnload:
        unload_asset( *reinterpret_cast< sAssetTask* >( _work.data ) );
        break;
    case eJobType::kRefresh:
        load_asset( *reinterpret_cast< sAssetTask* >( _work.data ), true );
        break;
    case eJobType::kPushEvent:
        push_event( *reinterpret_cast< sListenerTask* >( _work.data ) );
        break;
    }
}

void sk::Assets::Jobs::cAsset_Worker::load_asset( sAssetTask& _task, const bool _refresh )
{
    const auto loader_task = _refresh ? cAsset_Manager::eAssetTask::kRefreshAsset : cAsset_Manager::eAssetTask::kLoadAsset;
    auto assets = _task.loader( *_task.partial, loader_task );

    
}

void sk::Assets::Jobs::cAsset_Worker::unload_asset( sAssetTask& _task )
{
    _task.affected_assets += _task.loader( *_task.partial, cAsset_Manager::eAssetTask::kUnloadAsset );

    for( auto& asset : _task.affected_assets )
    {
        SK_FREE( asset->m_asset_ );
        asset->m_asset_ = nullptr;
    }
}

void sk::Assets::Jobs::cAsset_Worker::push_event( sListenerTask& _task )
{
    _task.event( *_task.partial, cAsset_Meta::eEventType::kLoaded );
}

