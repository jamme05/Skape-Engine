/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>
#include <Misc/Smart_Ptrs.h>
#include <Assets/Management/Asset_Manager.h>

namespace sk::Assets::Jobs
{
    using partial_t   = cShared_ptr< cAsset_Meta >;
    using void_ptr_t  = cShared_ptr< void >;
    using listener_t  = std::function< void( cAsset_Meta&, const void*, cAsset_Meta::eEventType ) >;
    using load_func_t = cAsset_Manager::load_file_func_t;

    enum class eJobType : uint8_t
    {
        kNone,
                
        kLoad,
        kUnload,
        kRefresh,
                
        kPushEvent,
    };

    struct sAssetTask
    {
        using path_t = std::filesystem::path;
        path_t        path;
        // Main asset being loaded
        partial_t     partial;
        // Additional assets being affected
        cAsset_List   affected_assets;
        load_func_t   loader;
        const void*   source;
    };
                
    struct sListenerTask
    {
        partial_t   partial;
        listener_t  event;
        const void* source;
    };
            
    struct sTask
    {
        eJobType type;
        void*    data;
    };
} // sk::Assets::Jobs::