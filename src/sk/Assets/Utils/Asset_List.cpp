/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_List.h"

#include <sk/Assets/Asset.h>

#include <utility>

namespace sk::Assets
{
	cAsset_List::~cAsset_List( void )
	{
		m_assets_.clear();
	} // ~cAsset_List

	cAsset_List::cAsset_List( const cAsset_List& _other )
	{
		m_assets_ = _other.m_assets_;
	} // cAsset_List Copy

	cAsset_List::cAsset_List( cAsset_List&& _other ) noexcept
	{
		m_assets_ = std::move( _other.m_assets_ );
	} // cAsset_List Move

	cAsset_List& cAsset_List::operator=( const cAsset_List& _other )
	{
		if( this != &_other )
			m_assets_ = _other.m_assets_;

		return *this;
	} // operator= ( Copy )

	cAsset_List& cAsset_List::operator=( cAsset_List&& _other ) noexcept
	{
		m_assets_ = std::move( _other.m_assets_ );
		return *this;
	} // operator= ( Move )

	cAsset_List& cAsset_List::operator+=( const cAsset_List& _other )
	{
		m_assets_.insert( _other.m_assets_.begin(), _other.m_assets_.end() );
		return *this;
	} // operator+= ( Copy )

	cAsset_List& cAsset_List::operator+=( cAsset_List&& _other )
	{
		m_assets_.merge( std::move( _other ).m_assets_ );
		return *this;
	} // operator+= ( Move )
	
	auto cAsset_List::GetAssetOfType( const iRuntimeClass& _class ) -> cShared_ptr< cAsset_Meta >
	{
		const auto [ fst, snd ] = m_assets_.equal_range( _class.getTypeHash() );

		if( fst == m_assets_.end() )
			return nullptr;

		// Always return same if only a single exists.
		if( fst == snd )
			return fst->second;

		auto& counter = m_counters_[ _class.getTypeHash() ];

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
		auto range = m_assets_.equal_range( _class.getTypeHash() );
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

	void cAsset_List::AddAsset( const cShared_ptr< cAsset_Meta >& _asset )
	{
		if( _asset == nullptr )
			return;
		
		m_assets_.insert( { _asset->GetHash(), _asset } );
	} // add_asset

	void cAsset_List::RemoveAsset( const cShared_ptr< cAsset_Meta >& _asset )
	{
		auto [ fst, snd ] = m_assets_.equal_range( _asset->GetHash() );
		for( auto it = fst; it != snd; ++it )
		{
			if( it->second == _asset )
				m_assets_.erase( it );
		}
	} // remove_asset

} // sk::Assets::

#undef GUARD