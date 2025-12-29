/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_List.h"

#include <utility>

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
	
	auto cAsset_List::GetAssetOfType( const iRuntimeClass& _class ) -> cShared_ptr< cAsset_Meta >
	{
		const auto [ fst, snd ] = m_assets.equal_range( _class.getType() );

		if( fst == m_assets.end() )
			return nullptr;

		// Always return same if only a single exists.
		if( fst == snd )
			return fst->second;

		auto& counter = m_counters[ _class.getType() ];

		if( const auto dist = std::distance( fst, snd ); static_cast< int_fast32_t >( counter ) >= dist )
		{
			SK_WARNING( sk::Severity::kEngine, 
				"Warning: No more assets of type %s. Asset List will now loop around.", _class.getRawName() )
			counter = 0;
		}
		const auto itr = std::next( fst, counter++ );

		return itr->second;
	}

	auto cAsset_List::GetAssetsOfType( const iRuntimeClass& _class, const int32_t _max_count )->std::vector<cShared_ptr<cAsset_Meta>>
	{
		auto range = m_assets.equal_range( _class.getType() );
		std::vector< cShared_ptr< cAsset_Meta > > assets;

		if( _max_count > 0 )
		{
			for( auto itr = range.first; itr != range.second; ++itr )
				assets.push_back( itr->second );
		}
		else
		{
			size_t count = 0;
			for( auto itr = range.first; itr != range.second && std::cmp_less( count, _max_count ); ++itr, ++count )
				assets.push_back( itr->second );
		}

		return assets;
	}

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