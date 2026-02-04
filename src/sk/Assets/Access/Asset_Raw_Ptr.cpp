/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Raw_Ptr.h"

using namespace sk;

cAsset_Raw_Ptr::cAsset_Raw_Ptr( const cWeak_Ptr< iClass >& _self )
: on_asset_loaded( _self )
, on_asset_updated( _self )
, on_asset_unloaded( _self )
{}

cAsset_Raw_Ptr::cAsset_Raw_Ptr( const cWeak_Ptr< iClass >& _self, const cShared_ptr< cAsset_Meta >& _meta )
: cAsset_Raw_Ptr( _self )
{
    if( _meta != nullptr )
        SetAsset( _meta );
}

cAsset_Raw_Ptr::cAsset_Raw_Ptr( const cAsset_Raw_Ptr& _other, const cWeak_Ptr< iClass >& _self )
: cAsset_Raw_Ptr( _self, _other.m_meta_.Lock() )
{}

cAsset_Raw_Ptr::cAsset_Raw_Ptr( cAsset_Raw_Ptr&& _other ) noexcept
: on_asset_loaded( std::move( _other.on_asset_loaded ) )
, on_asset_updated( std::move( _other.on_asset_updated ) )
, on_asset_unloaded( std::move( _other.on_asset_unloaded ) )
{
    cAsset_Ptr_Base::operator=( std::move( _other ) );
}

cAsset_Raw_Ptr::~cAsset_Raw_Ptr()
{
    if( IsLoaded() )
        SetAsset( nullptr );
}

cAsset_Raw_Ptr& cAsset_Raw_Ptr::operator=( const cAsset_Raw_Ptr& _other )
{
    if( &_other != this )
        cAsset_Ptr_Base::operator=( _other );

    return *this;
}

cAsset_Raw_Ptr& cAsset_Raw_Ptr::operator=( cAsset_Raw_Ptr&& _other ) noexcept
{
    cAsset_Ptr_Base::operator=( std::move( _other ) );

    return *this;
}

void cAsset_Raw_Ptr::subscribe()
{
    m_meta_->AddListener( CreateEvent( &cAsset_Raw_Ptr::on_asset_event ) );
}

void cAsset_Raw_Ptr::unsubscribe()
{
    m_meta_->RemoveListener( GetFunctionId( &cAsset_Raw_Ptr::on_asset_event ) );
}

bool cAsset_Raw_Ptr::_allowDirectLoad()
{
    return on_asset_loaded.size() == 0;
}

cAsset_Raw_Ptr::self_t cAsset_Raw_Ptr::get_self() const
{
    return on_asset_loaded.GetSelf();
}

auto cAsset_Raw_Ptr::validate_asset( const cShared_ptr< cAsset_Meta >& _meta ) const -> cShared_ptr< cAsset_Meta >
{
    return _meta;
}

void cAsset_Raw_Ptr::on_asset_event( cAsset_Meta& _meta, const Assets::eEventType _event )
{
    const auto asset = _meta.GetAsset();
    
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
