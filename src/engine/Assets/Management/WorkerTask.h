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
    using listener_t  = std::function< void( cAsset_Meta&, void*, cAsset_Meta::eEventType ) >;
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
        // Main asset being loaded
        partial_t     partial;
        // Additional assets being affected
        cAsset_List   affected_assets;
        load_func_t   loader;
    };
                
    struct sListenerTask
    {
        partial_t  partial;
        listener_t event;
    };
            
    struct sTask
    {
        using ptr_t = std::byte*;
        eJobType type;
        ptr_t    data;
    };
} // sk::Assets::Jobs::