/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>
#include <functional>

#include <Assets/Asset.h>
#include <Misc/Smart_Ptrs.h>

namespace sk::Assets::Jobs
{
    class cAsset_Job_Manager
    {
    public:
        using partial_t  = cShared_ptr< cAsset_Meta >;
        using void_ptr_t = cShared_ptr< void >;
        using listener_t = std::function< void( cAsset_Meta&, cAsset_Meta::eEventType ) >;

        enum class eJobType : uint8_t
        {
            kNone,
            
            kLoad,
            kUnload,
            kRefresh,
            
            kPushEvent,
            
            kUpdateExtensions,
        };

        struct sAssetTask
        {
            partial_t partial;
        };
            
        struct sListenerTask
        {
            partial_t  partial;
            listener_t event;
        };
        
        struct sTask
        {
            eJobType type;
            void*    data;
        };
        
    };
} // sk::
