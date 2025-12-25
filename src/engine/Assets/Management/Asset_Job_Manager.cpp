#include "Asset_Job_Manager.h"

#include <Assets/Management/Asset_Loader.h>

sk::Assets::Jobs::cAsset_Job_Manager::cAsset_Job_Manager()
{
    m_tasks_.resize( 128 );

    m_head_.store( 0 );
    m_tail_.store( 0 );
    
    // Magic numbers my beloved. But this should create a pretty balanced amount of asset loaders.
    const auto nr_of_loaders = std::max( 1u, std::thread::hardware_concurrency() / 3 );
    m_workers_.resize( nr_of_loaders );
    
    for( auto& worker : m_workers_ )
    {
        worker.m_working_.store( true );
        worker.m_thread_ = std::thread{ &cAsset_Worker::worker, &worker };
    }
}

sk::Assets::Jobs::cAsset_Job_Manager::~cAsset_Job_Manager()
{
    for( auto& worker : m_workers_ )
        worker.m_working_.store( false );

    m_head_.store( m_tasks_.size() + 1 );
    m_head_.notify_all();
    
    for( auto& worker : m_workers_ )
        worker.m_thread_.join();
}

auto sk::Assets::Jobs::cAsset_Job_Manager::WaitForTask( const std::atomic_bool& _working_ref ) -> sTask
{
    if( const auto tail = m_tail_.load(); tail == m_head_.load() )
        m_head_.wait( tail );

    // The worker is no longer active.
    if( !_working_ref.load() )
        return {};

    const auto new_tail = ++m_tail_;
    
    auto task = sTask{};
    std::swap( task, m_tasks_[ new_tail ] );

    return task;
}

void sk::Assets::Jobs::cAsset_Job_Manager::push_task( const sTask& _task )
{
    // TODO: Figure out a way to resize the task list.
    auto new_head = m_head_.load();
    new_head = ( new_head + 1 ) % m_tasks_.size();

    m_tasks_[ new_head ] = _task;

    m_head_.store( new_head );
    m_head_.notify_one();
}
