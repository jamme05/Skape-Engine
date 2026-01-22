/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Access/Asset_Ptr_Base.h>
#include <sk/Scene/Managers/EventManager.h>

#include <type_traits>

namespace sk
{
    class cAsset;
    class cAsset_Meta;
    class cAsset_Manager;
} // sk::

namespace sk
{
    enum class eAsset_Ref_Mode : uint8_t
    {
        kAutomaticAsync,
        kAutomaticSync,
        kManual
    };

    template< eAsset_Ref_Mode >
    class cAsset_Ref_Base : protected cAsset_Ptr_Base
    {
    public:
        cAsset_Ref_Base() : cAsset_Ptr_Base() {}
        
        using cAsset_Ptr_Base::IsLoaded;
        using cAsset_Ptr_Base::IsValid;
        using cAsset_Ptr_Base::WaitUntilLoaded;
    };

    template<>
    class cAsset_Ref_Base< eAsset_Ref_Mode::kManual > : public cAsset_Ptr_Base
    {
    public:
        cAsset_Ref_Base() : cAsset_Ptr_Base() {}
    };

    // Will keep the asset loading during its lifetime if mode it's mode is automatic. Otherwise, you'll get control.
    // The reason for this option is due to the asset ref being built to have a smaller footprint compared to the asset pointers.
    template< reflected Ty, eAsset_Ref_Mode Mode = eAsset_Ref_Mode::kAutomaticAsync >
        requires std::is_base_of_v< cAsset, Ty >
    class cAsset_Ref final : public cAsset_Ref_Base< Mode >
    {
        using ref_base_t = cAsset_Ref_Base< Mode >;
        using base_t     = cAsset_Ptr_Base;

    public:
        using dispatcher_t = Event::cDispatcherProxy< Assets::eEventType, cAsset_Ref< Ty, Mode >& >;

        static constexpr auto kType = type_info_t{ kTypeInfo< Ty > };
        static constexpr auto kMode = Mode; 
        
        cAsset_Ref() = default;
        cAsset_Ref( const cAsset_Ref& _other );
        cAsset_Ref( cAsset_Ref&& _other ) noexcept;
        cAsset_Ref( const cWeak_Ptr< iClass >& _self );
        cAsset_Ref( const cWeak_Ptr< iClass >& _self, const cShared_ptr< cAsset_Meta >& _meta );
        ~cAsset_Ref() override;

        auto operator=( const cAsset_Ref& _other ) -> cAsset_Ref&;
        auto operator=( cAsset_Ref&& _other ) noexcept -> cAsset_Ref&;
        auto operator=( const cShared_ptr< cAsset_Meta >& _meta ) -> cAsset_Ref&;
        auto operator=( std::nullptr_t ) -> cAsset_Ref&;
        
        operator bool() const;

        auto operator*() -> Ty&;
        auto operator*() const -> const Ty&;
        auto operator->() -> Ty*;
        auto operator->() const -> const Ty*;
        
        // Will hard check if the other reference is this.
        bool operator==( const cAsset_Ref& _other ) const;
        bool operator==( const cShared_ptr< cAsset_Meta >& _meta ) const;

        auto Get() -> Ty*;
        auto Get() const -> const Ty*;
        
        dispatcher_t on_changed;

    private:
        using self_t = cWeak_Ptr< iClass >;

        void try_load();
        void subscribe() override;
        void unsubscribe() override;
        auto get_self() const -> self_t override;
        auto validate_asset( const cShared_ptr< cAsset_Meta >& _meta ) const -> cShared_ptr< cAsset_Meta > override;

        void on_asset_event( cAsset_Meta& _meta, const Assets::eEventType _event ) override;
    };

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ref< Ty, Mode >::cAsset_Ref( const cAsset_Ref& _other )
    {
        base_t::operator=( _other );
        
        try_load();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ref< Ty, Mode >::cAsset_Ref( cAsset_Ref&& _other ) noexcept
    {
        base_t::operator=( std::move( _other ) );
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ref< Ty, Mode >::cAsset_Ref( const cWeak_Ptr< iClass >& _self )
    : on_changed( _self )
    {
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ref< Ty, Mode >::cAsset_Ref( const cWeak_Ptr< iClass >& _self, const cShared_ptr< cAsset_Meta >& _meta )
    : on_changed( _self )
    {
        base_t::SetAsset( _meta );
        try_load();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v<cAsset, Ty>
    cAsset_Ref< Ty, Mode >::~cAsset_Ref()
    {
        if( base_t::IsLoaded() )
            base_t::SetAsset( nullptr );
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator=( const cAsset_Ref& _other ) -> cAsset_Ref&
    {
        if( &_other != this )
            base_t::operator=( _other );

        try_load();

        return *this;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator=( cAsset_Ref&& _other ) noexcept -> cAsset_Ref&
    {
        base_t::operator=( std::move( _other ) );

        try_load();

        return *this;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator=( const cShared_ptr< cAsset_Meta >& _meta ) -> cAsset_Ref&
    {
        base_t::SetAsset( _meta );

        try_load();
        
        return *this;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator=( std::nullptr_t ) -> cAsset_Ref&
    {
        base_t::SetAsset( nullptr );
        
        return *this;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    cAsset_Ref< Ty, Mode >::operator bool() const
    {
        return base_t::IsLoaded();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator*() -> Ty&
    {
        return *Get();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator*() const -> const Ty&
    {
        return *Get();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator->() -> Ty*
    {
        return Get();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::operator->() const -> const Ty*
    {
        return Get();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    bool cAsset_Ref< Ty, Mode >::operator==( const cAsset_Ref& _other ) const
    {
        return this == &_other;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v<cAsset, Ty>
    bool cAsset_Ref< Ty, Mode >::operator==( const cShared_ptr< cAsset_Meta >& _meta ) const
    {
        return base_t::m_meta_ == _meta;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::Get() -> Ty*
    {
        return static_cast< Ty* >( base_t::GetAssetRaw() );
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::Get() const -> const Ty*
    {
        return static_cast< const Ty* >( base_t::GetAssetRaw() );
    }

    // Rider says this can be made static, but that is incorrect.
    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ref< Ty, Mode >::try_load()
    {
        if constexpr( Mode != eAsset_Ref_Mode::kManual )
        {
            if( base_t::IsLoaded() || !base_t::IsValid() )
                return;
        }
        if constexpr( Mode == eAsset_Ref_Mode::kAutomaticAsync )
        {
            base_t::LoadAsync();
        }
        else if constexpr( Mode == eAsset_Ref_Mode::kAutomaticSync )
        {
            base_t::LoadSync();
        }
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ref< Ty, Mode >::subscribe()
    {
        // TODO: Make the events really be sure of what is being deleted.
        base_t::m_meta_->AddListener( sk::CreateEvent( this, &cAsset_Ref::on_asset_event ) );
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ref< Ty, Mode >::unsubscribe()
    {
        base_t::m_meta_->RemoveListener( sk::CreateEvent( this, &cAsset_Ref::on_asset_event ) );
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::get_self() const -> self_t
    {
        return on_changed.GetSelf();
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    auto cAsset_Ref< Ty, Mode >::validate_asset(
        const cShared_ptr< cAsset_Meta >& _meta ) const -> cShared_ptr< cAsset_Meta >
    {
        if( _meta == nullptr )
            return _meta;
        
        SK_BREAK_RET_IF( sk::Severity::kEngine, kType != _meta->GetType(),
                         "Error: Asset type does not match the requested type!", nullptr )

        return _meta;
    }

    template< reflected Ty, eAsset_Ref_Mode Mode > requires std::is_base_of_v< cAsset, Ty >
    void cAsset_Ref< Ty, Mode >::on_asset_event( cAsset_Meta& _meta, const Assets::eEventType _event )
    {
        auto asset = static_cast< Ty* >( _meta.GetAsset() );
        switch( _event )
        {
        case Assets::eEventType::kUnload:
            base_t::m_asset_.store( nullptr );
            on_changed.push_event( Assets::eEventType::kUnload, *this );
            break;
        case Assets::eEventType::kLoaded:
            base_t::m_asset_.store( asset );
            on_changed.push_event( Assets::eEventType::kLoaded, *this );
            base_t::m_asset_.notify_all();
            break;
        case Assets::eEventType::kUpdated:
            base_t::m_asset_.store( asset );
            on_changed.push_event( Assets::eEventType::kUpdated, *this );
            break;
        }
    }
} // sk::
