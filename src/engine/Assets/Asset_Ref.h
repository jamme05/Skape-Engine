/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <type_traits>

#include <Scene/Managers/EventManager.h>

namespace sk
{
    class cAsset_Meta;
    class cAsset;

    class cAsset_Ref_Base
    {
        
    };
    
    // Will keep the asset loading during its lifetime
    template< class Ty >
    requires std::is_base_of_v< cAsset, Ty >
    class cAsset_Ref
    {
    public:
        enum class eUpdateType : uint8_t
        {
            kLoaded,
            kUpdated,
            kUnloaded
        };

        using dispatcher_t = Event::cDispatcherProxy< eUpdateType, Ty*, cAsset_Meta& >;

        
        dispatcher_t on_asset_changed;

        
    };
} // sk::
