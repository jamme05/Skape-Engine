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
    class cAsset_Ref: Event::cHelper< cAsset_Ref >
    {
        friend class cAsset_Manager;
        
    public:
        using listener_t          = cAsset_Meta::dispatcher_t::listener_t;
        using weak_listener_t     = cAsset_Meta::dispatcher_t::weak_listener_t;
        using load_dispatcher_t   = Event::cDispatcherProxy< bool, cAsset& >;
        using update_dispatcher_t = Event::cDispatcherProxy< cAsset& >;
        using unload_dispatcher_t = Event::cDispatcherProxy< bool, cAsset_Meta& >;

        cAsset_Ref() = default;
        explicit cAsset_Ref( const cShared_ptr< cAsset_Meta >& _meta, const cWeak_Ptr< iClass >& _self = nullptr );
        cAsset_Ref( const cAsset_Ref& _other, const cWeak_Ptr< iClass >& _self = nullptr );
        cAsset_Ref( cAsset_Ref&& _other ) noexcept;
        ~cAsset_Ref();

        cAsset_Ref& operator=( const cAsset_Ref& _other );
        cAsset_Ref& operator=( cAsset_Ref&& _other ) noexcept;

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

        auto get_source() const -> const void*;
        
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
