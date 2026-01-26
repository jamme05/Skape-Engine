/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Asset.h>
#include <sk/Assets/Workers/WorkerTask.h>
#include <sk/Misc/Smart_Ptrs.h>

#include <thread>


namespace sk
{
    class cAsset_Meta;
} // sk

namespace sk::Assets::Jobs
{
    class cAsset_Worker
    {
        friend class cAsset_Job_Manager;

    public:
        cAsset_Worker();
        using partial_t  = cShared_ptr< cAsset_Meta >;
        using void_ptr_t = cShared_ptr< void >;
        using listener_t = std::function< void( cAsset_Meta&, eEventType ) >;

    private:
        static void worker( cAsset_Worker* _loader );
        static void do_work( const sTask& _work );
        static void load_asset( sAssetTask& _task, bool _refresh );
        static void unload_asset( sAssetTask& _task );
        static void push_event( sListenerTask& _task );
        
        std::atomic_bool m_working_ = false;
        std::atomic_bool m_active_  = false;

        std::thread m_thread_;
    };
} // sk::
