/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>
#include <Misc/Smart_Ptrs.h>

#include <functional>

namespace sk
{
    class cAsset;
    class cAsset_Meta;
    class cAsset_Manager;
} // sk::

namespace sk
{
    // Allows manual loading and unloading of the asset.
    class cAsset_Ptr : Event::cHelper< cAsset_Ptr >
    {
        friend class cAsset_Manager;
        
    public:
        using listener_t          = cAsset_Meta::dispatcher_t::listener_t;
        using weak_listener_t     = cAsset_Meta::dispatcher_t::weak_listener_t;
        using load_dispatcher_t   = Event::cDispatcherProxy< bool, cAsset& >;
        using update_dispatcher_t = Event::cDispatcherProxy< cAsset& >;
        using unload_dispatcher_t = Event::cDispatcherProxy< bool, cAsset_Meta& >;

        cAsset_Ptr() = default;
        explicit cAsset_Ptr( const cShared_ptr< cAsset_Meta >& _meta, const cWeak_Ptr< iClass >& _self = nullptr );
        cAsset_Ptr( const cAsset_Ptr& _other, const cWeak_Ptr< iClass >& _self = nullptr );
        cAsset_Ptr( cAsset_Ptr&& _other ) noexcept;
        ~cAsset_Ptr();

        cAsset_Ptr& operator=( const cAsset_Ptr& _other );
        cAsset_Ptr& operator=( cAsset_Ptr&& _other ) noexcept;

        [[ nodiscard ]] auto GetAsset() const -> cAsset*;
        [[ nodiscard ]] bool IsLoaded() const;

        bool SetAsset( const cShared_ptr< cAsset_Meta >& _meta );

        auto LoadSync () -> cAsset*;
        bool LoadAsync();
        
        void Unload();

        bool IsValid() const;

        // Dispatchers
        load_dispatcher_t   on_asset_loaded;
        update_dispatcher_t on_asset_updated;
        unload_dispatcher_t on_asset_unloaded;
        
    private:
        using meta_t = cWeak_Ptr< cAsset_Meta >;
        using self_t = cWeak_Ptr< iClass >;
        
        void validate   () const;
        void subscribe  ();
        void unsubscribe();
        
        void on_asset_event( cAsset_Meta& _meta, const void* _source, cAsset_Meta::eEventType _event );

        self_t  m_self_        = nullptr;
        meta_t  m_asset_meta_  = nullptr;
        cAsset* m_asset_       = nullptr;
        
        bool             m_is_listening_ = false;
        std::atomic_bool m_has_loaded_   = false;
    };
} // sk::
