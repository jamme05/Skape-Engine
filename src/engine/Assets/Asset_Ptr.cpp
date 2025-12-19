/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Ptr.h"

sk::cAsset_Ptr::cAsset_Ptr( const cShared_ptr< cAsset_Meta >& _meta, const cWeak_Ptr< iClass >& _self )
: on_asset_loaded( _self )
, on_asset_updated( _self )
, on_asset_unloaded( _self )
, m_self_( _self )
{
    SetAsset( _meta );
}

sk::cAsset_Ptr::cAsset_Ptr( const cAsset_Ptr& _other, const cWeak_Ptr< iClass >& _self )
: m_self_( _self )
{
    SetAsset( _other.m_asset_meta_.Lock() );
}

sk::cAsset_Ptr::cAsset_Ptr( cAsset_Ptr&& _other ) noexcept
: on_asset_loaded( std::move( _other.on_asset_loaded ) )
, on_asset_updated( std::move( _other.on_asset_updated ) )
, on_asset_unloaded( std::move( _other.on_asset_unloaded ) )
, m_self_( std::move( _other.m_self_ ) )
, m_asset_( _other.m_asset_ )
{
    SetAsset( _other.m_asset_meta_.Lock() );
}

sk::cAsset_Ptr::~cAsset_Ptr()
{
    if( IsLoaded() )
        Unload();
}

sk::cAsset_Ptr& sk::cAsset_Ptr::operator=( const cAsset_Ptr& _other )
{
    
}

sk::cAsset_Ptr& sk::cAsset_Ptr::operator=( cAsset_Ptr&& _other ) noexcept
{
    
}

auto sk::cAsset_Ptr::GetAsset() const -> cAsset*
{
    return IsLoaded() ? m_asset_ : nullptr;
}

bool sk::cAsset_Ptr::IsLoaded() const
{
    return m_has_loaded_.load() && m_asset_meta_ != nullptr && m_asset_meta_->IsLoaded();
}

bool sk::cAsset_Ptr::SetAsset( const cShared_ptr< cAsset_Meta >& _meta )
{
    if( m_asset_meta_ != nullptr )
        unsubscribe();
    
    m_asset_meta_ = _meta;

    if( _meta == nullptr )
        return false;

    subscribe();
    return true;
}

auto sk::cAsset_Ptr::LoadSync() -> cAsset*
{
    if( IsLoaded() )
    {
        SK_WARNING( sk::Severity::kGeneral | 10,
            "Warning: Asset is already loaded by this reference." )
        
        return m_asset_;
    }
    
    LoadAsync();
    
    m_has_loaded_.wait( false );

    return m_asset_;
}

bool sk::cAsset_Ptr::LoadAsync()
{
    validate();

    m_asset_meta_->AddReferrer( this, m_self_ );
}

void sk::cAsset_Ptr::Unload()
{
    validate();

    if( !m_has_loaded_.load() )
    {
        SK_WARNING( sk::Severity::kGeneral | 10,
            "Warning: Trying to unload an already unloaded asset reference." )
        return;
    }

    m_has_loaded_.store( false );
    
    m_asset_meta_->RemoveReferrer( this, m_self_ );
}

bool sk::cAsset_Ptr::IsValid() const
{
    return m_asset_meta_ != nullptr;
}

void sk::cAsset_Ptr::validate() const
{
    SK_ERR_IF( m_asset_meta_ == nullptr,
        "Error: Trying to use an uninitialized asset reference!" )
}

void sk::cAsset_Ptr::subscribe()
{
    m_asset_meta_->AddListener( CreateEvent( &on_asset_event ) );
}

void sk::cAsset_Ptr::unsubscribe()
{
    m_asset_meta_->RemoveListener( GetFunctionId( &on_asset_event ) );
}

void sk::cAsset_Ptr::on_asset_event( cAsset_Meta& _meta, const void* _source, const cAsset_Meta::eEventType _event )
{
    const auto asset     = _meta.GetAsset();
    const bool from_this = _source == this;
    switch( _event )
    {
    case cAsset_Meta::eEventType::kLoaded:
        m_asset_ = asset;
        
        m_has_loaded_.store( true );
        m_has_loaded_.notify_all();
        
        on_asset_loaded.push_event( from_this, *asset );
    break;
    case cAsset_Meta::eEventType::kUpdated:
        on_asset_updated.push_event( *asset );
    break;
    case cAsset_Meta::eEventType::kUnload:
        on_asset_unloaded.push_event( from_this, _meta );
    break;
    }
}
