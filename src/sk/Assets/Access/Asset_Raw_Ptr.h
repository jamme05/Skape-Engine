/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>
#include <Assets/Access/Asset_Ptr_Base.h>
#include <Scene/Managers/EventManager.h>

namespace sk
{
    class cAsset;
    class cAsset_Meta;
    class cAsset_Manager;
} // sk::

namespace sk
{
    // Allows manual loading and unloading of the asset.
    class cAsset_Raw_Ptr final : public cAsset_Ptr_Base, public Event::cHelper< cAsset_Raw_Ptr >
    {
        friend class cAsset_Manager;
        
    public:
        using load_dispatcher_t   = Event::cDispatcherProxy< cAsset& >;
        using update_dispatcher_t = Event::cDispatcherProxy< cAsset& >;
        using unload_dispatcher_t = Event::cDispatcherProxy< cAsset_Meta& >;

        // TODO: Make the default constructor invalid.
        cAsset_Raw_Ptr() = default;
        explicit cAsset_Raw_Ptr( const cWeak_Ptr< iClass >& _self );
        explicit cAsset_Raw_Ptr( const cWeak_Ptr< iClass >& _self, const cShared_ptr< cAsset_Meta >& _meta );
        cAsset_Raw_Ptr( const cAsset_Raw_Ptr& _other, const cWeak_Ptr< iClass >& _self = nullptr );
        cAsset_Raw_Ptr( cAsset_Raw_Ptr&& _other ) noexcept;
        ~cAsset_Raw_Ptr() override;

        cAsset_Raw_Ptr& operator=( const cAsset_Raw_Ptr& _other );
        cAsset_Raw_Ptr& operator=( cAsset_Raw_Ptr&& _other ) noexcept;
        
        // Dispatchers
        load_dispatcher_t   on_asset_loaded;
        update_dispatcher_t on_asset_updated;
        unload_dispatcher_t on_asset_unloaded;
        
    private:
        using meta_t = cWeak_Ptr< cAsset_Meta >;
        using self_t = cWeak_Ptr< iClass >;
        
        void subscribe  () override;
        void unsubscribe() override;
        auto get_self() const -> self_t override;
        auto validate_asset( const cShared_ptr< cAsset_Meta >& _meta ) const
            -> cShared_ptr< cAsset_Meta > override;
        
        void on_asset_event( cAsset_Meta& _meta, Assets::eEventType _event ) override;
    };
} // sk::
