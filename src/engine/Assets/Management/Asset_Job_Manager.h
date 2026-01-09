/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Workers/WorkerTask.h>
#include <Assets/Management/Asset_Manager.h>

namespace sk::Assets::Jobs
{
    class cAsset_Job_Manager : public cSingleton< cAsset_Job_Manager >
    {
        friend class sk::cAsset_Manager;
        friend class sk::cAsset_Meta;
    public:
        cAsset_Job_Manager();
        ~cAsset_Job_Manager() override;
        
        void Sync();

        auto WaitForTask( const std::atomic_bool& _working_ref ) -> sTask;
        
        auto GetWorkerCount() const -> size_t;
        
        bool IsShuttingDown() const;

    private:
        void push_task( const sTask& _task );
        void resize   ( size_t _new_size );
        auto distance () const -> size_t;

        using workers_t = cAsset_Worker*;
        
        std::vector< sTask > m_tasks_;
        
        std::atomic_size_t m_head_;
        std::atomic_size_t m_tail_;
        
        std::atomic_uint32_t m_available_;
        
        std::atomic_uint16_t m_currently_getting_work_;
        std::atomic_bool     m_currently_resizing_;
        std::atomic_bool     m_paused_;
        std::atomic_bool     m_shutting_down_;

        size_t      m_worker_count_;
		workers_t   m_workers_;
    };
} // sk::Assets::Jobs::
