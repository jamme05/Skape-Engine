/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Smart_Ptrs.h>

#include "Assets/Utils/Event.h"

namespace sk
{
    class cAsset;
    class cAsset_Meta;
    class cAsset_Manager;
    class iClass;
} // sk::

namespace sk
{
    // Base class with most logic required to get any derived asset ptr/ref class to function.
    class cAsset_Ptr_Base
    {
    public:
        cAsset_Ptr_Base() = default;
        explicit cAsset_Ptr_Base( const cShared_ptr< cAsset_Meta >& _meta );
        cAsset_Ptr_Base( const cAsset_Ptr_Base& _other );
        virtual ~cAsset_Ptr_Base();

        auto operator=( const cAsset_Ptr_Base& _other ) -> cAsset_Ptr_Base&;
        auto operator=( cAsset_Ptr_Base&& _other ) noexcept -> cAsset_Ptr_Base&;

        [[ nodiscard ]] auto GetAssetRaw() const -> cAsset*;
        [[ nodiscard ]] auto GetMeta() const -> cShared_ptr< cAsset_Meta >;
        [[ nodiscard ]] bool IsLoaded() const;

        bool SetAsset( const cShared_ptr< cAsset_Meta >& _meta );

        // TODO: Add try load functions. The try load functions will not complain if the asset is already loaded.
        auto LoadSync() -> cAsset*;
        bool LoadAsync();

        void WaitUntilLoaded() const;
        void Unload();

        virtual bool IsValid() const;

    protected:
        using meta_t = cWeak_Ptr< cAsset_Meta >;
        using self_t = cWeak_Ptr< iClass >;
        using asset_ptr_t = std::atomic< cAsset* >;

        bool has_data() const;
        void validate() const;
        virtual void subscribe  () = 0;
        virtual void unsubscribe() = 0;
        virtual auto get_self() const -> self_t = 0;

        // Return nullptr if the asset is invalid. Otherwise, simply forward the pointer.
        virtual auto validate_asset( const cShared_ptr< cAsset_Meta >& _meta ) const
            -> cShared_ptr< cAsset_Meta > = 0;
        
        virtual void on_asset_event( cAsset_Meta& _meta, Assets::eEventType _event ) = 0;
        
        meta_t      m_meta_  = nullptr;
        asset_ptr_t m_asset_ = nullptr;
    };
} // sk::
