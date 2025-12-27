/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Containers/Map.h>

#include <Reflection/Type_Hash.h>

#include <Misc/Smart_Ptrs.h>

#include <fastgltf/core.hpp>

namespace sk
{
	class cAsset;
	class cAsset_Meta;
	class cAsset_Manager;
} // sk::

namespace sk::Assets
{
	// A container for the newly imported assets.
	class cAsset_List
	{
		typedef multimap< type_hash, cShared_ptr< cAsset_Meta > > asset_map_t;
		typedef map< type_hash, size_t >                     asset_counter_map_t;

		class asset_iterator
		{
		public:
			asset_iterator( const asset_map_t::const_iterator& _it ) : m_it_( _it ){}

			const cShared_ptr< cAsset_Meta >& operator*() const { return m_it_->second; }
			operator cShared_ptr< cAsset_Meta >        () const { return m_it_->second; }
			
			auto operator->() const -> const cShared_ptr< cAsset_Meta >& { return m_it_->second; }

			bool operator!=( const asset_iterator& _other ) const { return m_it_ != _other.m_it_; }
			bool operator==( const asset_iterator& _other ) const { return m_it_ == _other.m_it_; }

			asset_iterator& operator++(){ ++m_it_; return *this; }
			asset_iterator& operator--(){ ++m_it_; return *this; }
		private:
			asset_map_t::const_iterator m_it_;
		};

	public:
		 cAsset_List( void ) = default;
		~cAsset_List( void );

		cAsset_List( const cAsset_List& _other );
		cAsset_List( cAsset_List&& _other ) noexcept;

		cAsset_List& operator=( const cAsset_List& _other );
		cAsset_List& operator=( cAsset_List&& _other ) noexcept;

		cAsset_List& operator+=( const cAsset_List& _other );
		cAsset_List& operator+=( cAsset_List&& _other );

		template< class Ty >
		requires std::is_base_of_v< cAsset, Ty >
		auto Get_Asset_Of_Type() -> cShared_ptr< cAsset >
		{
			auto range = m_assets.equal_range( Ty::getStaticType() );

			if( range.first == m_assets.end() )
				return nullptr;

			// Always return same if only a single exists.
			if( range.first == range.second )
				return range.first->second;

			auto& counter = m_counters[ Ty::getStaticType() ];
			auto  dist    = std::distance( range.first, range.second );

			if( counter >= dist )
			{
				printf( "Warning: No more assets of type %s. Asset List will now loop around.", Ty::getStaticClass().getRawName() );
				counter = 0;
			}
			auto itr = std::next( range.first, counter++ );

			return itr->second;
		} // Get_Asset_Of_Type

		template< class Ty >
		requires std::is_base_of_v< cAsset, Ty >
		auto Get_Assets_Of_Type( const int32_t _max_count = 0 ) -> std::vector< cShared_ptr< Ty > >
		{
			auto range = m_assets.equal_range( Ty::getStaticClassType() );
			std::vector< cShared_ptr< Ty > > assets;

			if( _max_count > 0 )
			{
				for( auto itr = range.first; itr != range.second; ++itr )
					assets.push_back( itr->second );
			}
			else
			{
				size_t count = 0;
				for( auto itr = range.first; itr != range.second && count < _max_count; ++itr, ++count )
					assets.push_back( itr->second );
			}

			return assets;
		} // Get_Assets_Of_Type

		template< class Ty >
		requires std::is_base_of_v< cAsset, Ty >
		auto GetRange()
		{
			return m_assets.equal_range( Ty::getStaticType() );
		}

		[[ nodiscard ]] auto begin( void ) const { return asset_iterator( m_assets.begin() ); }
		[[ nodiscard ]] auto end  ( void ) const { return asset_iterator( m_assets.end  () ); }

		[[ nodiscard ]] bool empty( void ) const { return m_assets.empty(); }

	protected:

		void addAsset   ( const cShared_ptr< cAsset_Meta >& _asset );
		void removeAsset( const cShared_ptr< cAsset_Meta >& _asset );

	private:
		friend class sk::cAsset_Manager;

		asset_map_t         m_assets;
		asset_counter_map_t m_counters;
	};

} // sk::Assets::
