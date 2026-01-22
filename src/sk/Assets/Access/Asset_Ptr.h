/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Access/Asset_Ptr_Base.h>

namespace sk
{
    class cAsset;
    class cAsset_Meta;
    class cAsset_Manager;
} // sk::

namespace sk
{
    // Allows manual loading and unloading of the asset.
    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    class cAsset_Ptr final : public cAsset_Ptr_Base
    {
        friend class cAsset_Manager;
        using base_t = cAsset_Ptr_Base;

    public:
        using load_dispatcher_t   = Event::cDispatcherProxy< Ty& >;
        using update_dispatcher_t = Event::cDispatcherProxy< Ty& >;
        using unload_dispatcher_t = Event::cDispatcherProxy< cAsset_Meta& >;
        
        static constexpr type_info_t kType = kTypeInfo< Ty >;

        cAsset_Ptr() = default;
        explicit cAsset_Ptr( const cWeak_Ptr< iClass >& _self );
        cAsset_Ptr( const cWeak_Ptr< iClass >& _self, const cShared_ptr< cAsset_Meta >& _meta );
        cAsset_Ptr( const cAsset_Ptr& _other, const cWeak_Ptr< iClass >& _self = nullptr );
        cAsset_Ptr( cAsset_Ptr&& _other ) noexcept;
        ~cAsset_Ptr() override;

        cAsset_Ptr& operator=( const cAsset_Ptr& _other );
        cAsset_Ptr& operator=( cAsset_Ptr&& _other ) noexcept;

        [[ nodiscard ]] auto GetAsset() const -> Ty*;

        // Dispatchers
        load_dispatcher_t   on_asset_loaded;
        update_dispatcher_t on_asset_updated;
        unload_dispatcher_t on_asset_unloaded;

    private:
        using self_t = cWeak_Ptr< iClass >;

        auto validate_asset( const cShared_ptr< cAsset_Meta >& _meta ) const
            -> cShared_ptr< cAsset_Meta > override;
        void subscribe() override;
        void unsubscribe() override;
        auto get_self() const -> self_t override;

        void on_asset_event( cAsset_Meta& _meta, Assets::eEventType _event ) override;
    };

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ptr< Ty >::cAsset_Ptr( const cWeak_Ptr< iClass >& _self )
    : on_asset_loaded( _self )
    , on_asset_updated( _self )
    , on_asset_unloaded( _self )
    {}
    
    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ptr< Ty >::cAsset_Ptr( const cWeak_Ptr< iClass >& _self, const cShared_ptr< cAsset_Meta >& _meta )
    : base_t( _meta )
    , on_asset_loaded( _self )
    , on_asset_updated( _self )
    , on_asset_unloaded( _self )
    {}

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ptr< Ty >::cAsset_Ptr( const cAsset_Ptr& _other, const cWeak_Ptr< iClass >& _self )
    : cAsset_Ptr( _self, _other.m_meta_.Lock() )
    {}

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ptr< Ty >::cAsset_Ptr( cAsset_Ptr&& _other ) noexcept
    : on_asset_loaded( std::move( _other.on_asset_loaded ) )
    , on_asset_updated( std::move( _other.on_asset_updated ) )
    , on_asset_unloaded( std::move( _other.on_asset_unloaded ) )
    {
        base_t::operator=( std::move( _other ) );
    }

    template< reflected Ty > requires std::is_base_of_v<cAsset, Ty>
    cAsset_Ptr< Ty >::~cAsset_Ptr()
    {
        if( IsLoaded() )
            SetAsset( nullptr );
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ptr< Ty >& cAsset_Ptr< Ty >::operator=( const cAsset_Ptr& _other )
    {
        if( &_other != this )
            base_t::operator=( _other );
        
        return *this;
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ptr< Ty >& cAsset_Ptr< Ty >::operator=( cAsset_Ptr&& _other ) noexcept
    {
        base_t::operator=( std::move( _other ) );
        
        return *this;
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ptr< Ty >::GetAsset() const -> Ty*
    {
        return static_cast< Ty* >( GetAssetRaw() );
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ptr< Ty >::validate_asset( const cShared_ptr< cAsset_Meta >& _meta ) const -> cShared_ptr< cAsset_Meta >
    {
        if( _meta == nullptr )
            return nullptr;
        
        SK_BREAK_RET_IF( sk::Severity::kEngine, kType != _meta->GetType(),
            "Error: Asset type does not match the requested type!", nullptr )

        return _meta;
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ptr< Ty >::subscribe()
    {
        m_meta_->AddListener( sk::CreateEvent( this, &cAsset_Ptr::on_asset_event ) );
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ptr< Ty >::unsubscribe()
    {
        m_meta_->RemoveListener( sk::CreateEvent( this, &cAsset_Ptr::on_asset_event ) );
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ptr< Ty >::get_self() const -> self_t
    {
        return on_asset_loaded.GetSelf();
    }

    template< reflected Ty > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ptr< Ty >::on_asset_event( cAsset_Meta& _meta, const Assets::eEventType _event )
    {
        const auto asset = static_cast< Ty* >( _meta.GetAsset() );
        switch( _event )
        {
        case Assets::eEventType::kLoaded:
            m_asset_.store( asset );
        
            on_asset_loaded.push_event( *asset );
            
            m_asset_.notify_all();
            break;
        case Assets::eEventType::kUpdated:
            m_asset_.store( asset );
            
            on_asset_updated.push_event( *asset );
            break;
        case Assets::eEventType::kUnload:
            m_asset_.store( nullptr );
            on_asset_unloaded.push_event( _meta );
            break;
        }
    }
} // sk::
