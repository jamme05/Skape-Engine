/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <list>
#include <thread>

#include <Misc/Smart_Ptrs.h>
#include <Assets/Asset.h>

namespace sk
{
    class cAsset_Meta;
} // sk

namespace sk::Assets::Jobs
{
    class cAsset_Worker
    {
        friend class cAsset_Manager;

        cAsset_Worker();
    public:
        using partial_t  = cShared_ptr< cAsset_Meta >;
        using void_ptr_t = cShared_ptr< void >;
        using listener_t = std::function< void( cAsset_Meta&, cAsset_Meta::eEventType ) >;
        
        void AddAssetTask( eJobType _type, const partial_t& _asset );
        void AddListenerEvent( const partial_t& _asset, const listener_t& _listener );
        
        void UpdateExtensions( const unordered_map< str_hash, cAsset_Manager::load_file_func_t >& _new_map );

    private:
        void push_task( sTask&& _task );
        
        static void worker ( cAsset_Worker* _loader );
        static void do_work( cAsset_Worker& _self, sTask& _task );

        using extension_loader_map_t = unordered_map< str_hash, cAsset_Manager::load_file_func_t >;
        
        std::list< sTask >   m_queue_;
        std::atomic_bool     m_working_      = false;
        std::atomic_bool     m_queue_locked_ = false;
        std::atomic_uint32_t m_queue_size_   = 0;

        extension_loader_map_t m_extension_map_;
        extension_loader_map_t m_new_extension_map_;
        
        std::thread m_thread_;
    };
} // sk::
