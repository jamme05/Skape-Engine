/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_List.h"

#include "Assets/Asset.h"

namespace sk::Assets
{
	cAsset_List::~cAsset_List( void )
	{
		m_assets.clear();
	} // ~cAsset_List

	cAsset_List::cAsset_List( const cAsset_List& _other )
	{
		m_assets = _other.m_assets;
	} // cAsset_List Copy

	cAsset_List::cAsset_List( cAsset_List&& _other ) noexcept
	{
		m_assets = std::move( _other.m_assets );
	} // cAsset_List Move

	cAsset_List& cAsset_List::operator=( const cAsset_List& _other )
	{
		if( this != &_other )
			m_assets = _other.m_assets;

		return *this;
	} // operator= ( Copy )

	cAsset_List& cAsset_List::operator=( cAsset_List&& _other ) noexcept
	{
		m_assets = std::move( _other.m_assets );
		return *this;
	} // operator= ( Move )

	cAsset_List& cAsset_List::operator+=( const cAsset_List& _other )
	{
		m_assets.insert( _other.m_assets.begin(), _other.m_assets.end() );
		return *this;
	} // operator+= ( Copy )

	cAsset_List& cAsset_List::operator+=( cAsset_List&& _other )
	{
		m_assets.merge( std::move( _other ).m_assets );
		return *this;
	} // operator+= ( Move )

	void cAsset_List::addAsset( const cShared_ptr< cAsset_Meta >& _asset )
	{
		if( _asset == nullptr )
			return;
		
		m_assets.insert( { _asset->GetHash(), _asset } );
	} // add_asset

	void cAsset_List::removeAsset( const cShared_ptr< cAsset_Meta >& _asset )
	{
		auto [ fst, snd ] = m_assets.equal_range( _asset->GetHash() );
		for( auto it = fst; it != snd; ++it )
		{
			if( it->second == _asset )
				m_assets.erase( it );
		}
	} // remove_asset

} // sk::Assets::

#undef GUARD