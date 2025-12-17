/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_List.h"

#include "Assets/Asset.h"

#define GUARD std::scoped_lock lock( m_mtx );

namespace sk::Assets
{
	cAsset_List::~cAsset_List( void )
	{
		GUARD
		m_assets.clear();
	} // ~cAsset_List

	cAsset_List::cAsset_List( const cAsset_List& _other )
	{
		GUARD
		m_assets = _other.m_assets;
	} // cAsset_List Copy

	cAsset_List::cAsset_List( cAsset_List&& _other ) noexcept
	{
		GUARD
		m_assets = std::move( _other.m_assets );
	} // cAsset_List Move

	cAsset_List& cAsset_List::operator=( const cAsset_List& _other )
	{
		GUARD
		if( this != &_other )
			m_assets = _other.m_assets;

		return *this;
	} // operator= ( Copy )

	cAsset_List& cAsset_List::operator=( cAsset_List&& _other ) noexcept
	{
		GUARD
		m_assets = std::move( _other.m_assets );
		return *this;
	} // operator= ( Move )

	cAsset_List& cAsset_List::operator+=( const cAsset_List& _other )
	{
		GUARD
		m_assets.insert( _other.m_assets.begin(), _other.m_assets.end() );
		return *this;
	} // operator+= ( Copy )

	cAsset_List& cAsset_List::operator+=( cAsset_List&& _other )
	{
		GUARD
		m_assets.merge( std::move( _other ).m_assets );
		return *this;
	} // operator+= ( Move )

	void cAsset_List::add_asset( const cShared_ptr< cAsset_Meta >& _asset )
	{
		GUARD
		m_assets.insert( { _asset->getClassType(), _asset } );
	} // add_asset

	void cAsset_List::remove_asset( const cShared_ptr<cAsset_Meta>& _asset )
	{
		GUARD
		auto [ fst, snd ] = m_assets.equal_range( _asset->getClassType() );
		for( auto it = fst; it != snd; ++it )
		{
			if( it->second == _asset )
				m_assets.erase( it );
		}
	} // remove_asset

} // sk::Assets::

#undef GUARD