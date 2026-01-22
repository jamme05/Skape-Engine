

#include "Asset_Job_Manager.h"

#include <sk/Assets/Workers/Asset_Loader.h>
#include <sk/Graphics/Renderer.h>

#include <chrono>

sk::Assets::Jobs::cAsset_Job_Manager::cAsset_Job_Manager()
{
    m_tasks_.resize( 64 );

    m_head_.store( 0 );
    m_tail_.store( 0 );
    
    // Magic numbers my beloved. But this should create a pretty balanced amount of asset loaders.
    m_worker_count_ = std::max( 1u, std::thread::hardware_concurrency() / 3 );
    m_workers_ = SK_NEW( cAsset_Worker, m_worker_count_ );
    
    for( size_t i = 0; i < m_worker_count_; ++i )
    {
        auto& worker = m_workers_[ i ];
        worker.m_working_.store( true );
        worker.m_thread_ = std::thread{ &cAsset_Worker::worker, &worker };
    }
}

sk::Assets::Jobs::cAsset_Job_Manager::~cAsset_Job_Manager()
{
    Sync();
    
    for( size_t i = 0; i < m_worker_count_; ++i )
        m_workers_[ i ].m_working_.store( false );

    m_head_.store( m_tasks_.size() + 1 );
    m_head_.notify_all();
    
    for( size_t i = 0; i < m_worker_count_; ++i )
        m_workers_[ i ].m_thread_.join();
}

void sk::Assets::Jobs::cAsset_Job_Manager::Sync()
{
    for( auto tail = m_tail_.load(); tail != m_head_.load(); tail = m_tail_.load() )
    {
        Graphics::cRenderer::get().Update();
        std::this_thread::sleep_for( std::chrono::milliseconds{ 10 } );
    }
}

auto sk::Assets::Jobs::cAsset_Job_Manager::WaitForTask( const std::atomic_bool& _working_ref ) -> sTask
{
    if( m_currently_resizing_.load() )
        m_currently_resizing_.wait( true );
    
    if( const auto tail = m_tail_.load(); tail == m_head_.load() )
        m_head_.wait( tail );

    // The worker is no longer active.
    if( !_working_ref.load() )
        return {};

    ++m_currently_getting_work_;
    
    if( m_available_.load() != 0 )
        --m_available_;
    
    auto new_tail = ++m_tail_;
    if( new_tail >= m_tasks_.size() )
    {
        new_tail = 0;
        m_tail_.store( new_tail );
    }
    m_tail_.notify_one();
    
    auto task = sTask{};
    std::swap( task, m_tasks_[ new_tail ] );
    
    --m_currently_getting_work_;
    m_currently_getting_work_.notify_one();

    return task;
}

auto sk::Assets::Jobs::cAsset_Job_Manager::GetWorkerCount() const -> size_t
{
    return m_worker_count_;
}

bool sk::Assets::Jobs::cAsset_Job_Manager::IsShuttingDown() const
{
    return m_shutting_down_.load();
}

void sk::Assets::Jobs::cAsset_Job_Manager::push_task( const sTask& _task )
{
    // TODO: Figure out a way to resize the task list.
    const auto requested_head = m_head_.load() + 1;
    
    auto new_head = requested_head % m_tasks_.size();
    
    if( new_head == m_tail_.load() )
        resize( m_tasks_.size() * 2 );
    else if( const auto half_size = m_tasks_.size() / 2; m_available_.load() < half_size && half_size > 64 )
        resize( std::max< size_t >( half_size, distance() ) );
    
    new_head = requested_head % m_tasks_.size();

    m_tasks_[ new_head ] = _task;
    
    ++m_available_;
    
    m_head_.store( new_head );
    m_head_.notify_one();
}

void sk::Assets::Jobs::cAsset_Job_Manager::resize( const size_t _new_size )
{
    // We wait for the workers to complete their current tasks.
    // TODO: Move the head and tail when resizing.
    if( const auto working = m_currently_getting_work_.load(); working != 0 )
        m_currently_getting_work_.wait( working );
    
    m_currently_resizing_.store( true );
    
    std::vector< sTask > new_vec( _new_size );
    const auto head = m_head_.load();
    auto tail = m_tail_.load();
    
    // TODO: Figure out why the distance between the head and the tail is so huge.
    for( size_t i = 0; tail != head; ++tail, i++ )
    {
        tail = tail % m_tasks_.size();
        new_vec[ i ] = m_tasks_[ tail ];
    }
    m_tasks_.swap( new_vec );
    m_tail_.store( 0 );
    
    m_currently_resizing_.store( false );
    m_currently_resizing_.notify_all();
}

auto sk::Assets::Jobs::cAsset_Job_Manager::distance() const -> size_t
{
    const auto head = m_head_.load();
    const auto tail = m_tail_.load();
    if( head > tail )
        return std::abs( static_cast< int64_t >( head ) - static_cast< int64_t >( tail ) );
    
    return m_tasks_.size() - tail + head;
}
