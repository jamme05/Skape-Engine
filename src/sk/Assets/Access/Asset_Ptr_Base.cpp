/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Ptr_Base.h"

#include <Assets/Asset.h>
#include <Graphics/Renderer.h>

using namespace sk;

namespace
{
    auto has_requested_ptr_ = reinterpret_cast< cAsset* >( 1 );
} // ::

cAsset_Ptr_Base::cAsset_Ptr_Base( const cShared_ptr< cAsset_Meta >& _meta )
{
    if( _meta != nullptr )
        SetAsset( _meta );
}

cAsset_Ptr_Base::cAsset_Ptr_Base( const cAsset_Ptr_Base& _other )
{
    SetAsset( _other.m_meta_.Lock() );
}

cAsset_Ptr_Base::~cAsset_Ptr_Base()
{
    SK_BREAK_IF( sk::Severity::kEngine,
        IsLoaded(), "You HAVE to unload the asset ptr inside of the derived class. This is due to the virtual table" )
}

cAsset_Ptr_Base& cAsset_Ptr_Base::operator=( const cAsset_Ptr_Base& _other )
{
    if( &_other == this )
        return *this;
    
    SetAsset( _other.m_meta_.Lock() );
    m_asset_.store( _other.m_asset_ );

    return *this;
}

cAsset_Ptr_Base& cAsset_Ptr_Base::operator=( cAsset_Ptr_Base&& _other ) noexcept
{
    // TODO: Add some swap or better steal functionality.
    SetAsset( _other.m_meta_.Lock() );
    m_asset_.store( _other.m_asset_ );
    if( _other.IsLoaded() || m_asset_.load() == has_requested_ptr_ )
        LoadAsync();
    _other.SetAsset( nullptr );

    return *this;
}

auto cAsset_Ptr_Base::GetAssetRaw() const -> cAsset*
{
    return ( m_meta_->IsLoaded() && has_data() ) ? m_asset_.load() : nullptr;
}

auto cAsset_Ptr_Base::GetMeta() const -> cShared_ptr< cAsset_Meta >
{
    return m_meta_.Lock();
}

bool cAsset_Ptr_Base::IsLoaded() const
{
    return IsValid() && m_meta_->IsLoaded() && has_data();
}

bool cAsset_Ptr_Base::SetAsset( const cShared_ptr< cAsset_Meta >& _meta )
{
    if( IsLoaded() )
        Unload();
    
    if( m_meta_ != _meta && m_meta_.is_valid() )
        unsubscribe();
    
    m_meta_ = validate_asset( _meta );
    
    if( _meta == nullptr )
        return false;
    
    return true;
}

auto cAsset_Ptr_Base::LoadSync() -> cAsset*
{
    if( m_asset_.load() == has_requested_ptr_ )
    {
        SK_WARNING( sk::Severity::kGeneral | 10,
            "Warning: Asset is already requested by this asset pointer." )
        
        return m_asset_;
    }
    
    LoadAsync();
    
    m_asset_.wait( nullptr );

    return m_asset_;
}

bool cAsset_Ptr_Base::LoadAsync()
{
    validate();

    m_asset_.store( has_requested_ptr_ );
    subscribe();
    m_meta_->addReferrer( this, get_self() );

    return true;
}

void cAsset_Ptr_Base::WaitUntilLoaded() const
{
    if( const auto asset = m_asset_.load(); asset == has_requested_ptr_ )
    {
        // TODO: Make this prettier
        while( !IsLoaded() )
        {
            Graphics::cRenderer::get().Update();
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
    }
    else if( asset == nullptr )
        SK_WARNING( sk::Severity::kEngine, "Warning: Tried to wait for asset not requested to be loaded." )
}

void cAsset_Ptr_Base::Unload()
{
    validate();

    if( !has_data() )
    {
        SK_WARNING( sk::Severity::kGeneral | 10,
            "Warning: Trying to unload an already unloaded asset reference." )
        return;
    }

    m_asset_.store( nullptr );
    
    m_meta_->removeReferrer( this, get_self() );
}

bool cAsset_Ptr_Base::IsValid() const
{
    return m_meta_.is_valid();
}

bool cAsset_Ptr_Base::has_data() const
{
    const auto ptr = m_asset_.load();
    return ptr != nullptr && ptr != has_requested_ptr_;
}

void cAsset_Ptr_Base::validate() const
{
    SK_ERR_IF( m_meta_ == nullptr,
        "Error: Trying to use an uninitialized asset reference!" )
}
