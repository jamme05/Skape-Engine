/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Asset.h>
#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Misc/Smart_Ptrs.h>

namespace sk::Assets::Jobs
{
    using partial_t   = cShared_ptr< cAsset_Meta >;
    using void_ptr_t  = cShared_ptr< void >;
    using listener_t  = std::function< void( cAsset_Meta&, eEventType ) >;
    using load_func_t = cAsset_Manager::load_file_func_t;

    enum class eJobType : uint8_t
    {
        kNone,
                
        kLoad,
        kUnload,
        kRefresh,
        kSave,
                
        kPushEvent,
    };

    struct sAssetTask
    {
        using path_t = std::filesystem::path;
        path_t        path;
        // All assets being affected
        cAsset_List   affected_assets;
        load_func_t   loader;
    };
                
    struct sListenerTask
    {
        partial_t   meta;
        listener_t  event;
    };
            
    struct sTask
    {
        eJobType type;
        void*    data;
    };
} // sk::Assets::Jobs::