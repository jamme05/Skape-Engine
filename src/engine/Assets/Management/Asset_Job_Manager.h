/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Management/WorkerTask.h>
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

        auto WaitForTask( const std::atomic_bool& _working_ref ) -> sTask;

    private:
        void push_task( const sTask& _task );
        
		using workers_vector_t = vector< cAsset_Worker >;
        
        std::vector< sTask > m_tasks_;
        
        std::atomic_size_t m_head_;
        std::atomic_size_t m_tail_;

		workers_vector_t   m_workers_;
    };
} // sk::Assets::Jobs::
