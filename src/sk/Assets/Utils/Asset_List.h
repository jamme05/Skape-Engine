/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Containers/Map.h>
#include <sk/Misc/Smart_Ptrs.h>
#include <sk/Reflection/Type_Hash.h>

#include <vector>

namespace sk
{
	class iRuntimeClass;
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
		typedef map< type_hash, uint32_t >                        asset_counter_map_t;

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
		auto GetAssetOfType() -> cShared_ptr< cAsset_Meta >
		{
			return GetAssetOfType( Ty::getStaticClass() );
		} // Get_Asset_Of_Type
		
		auto GetAssetOfType( const iRuntimeClass& _class ) -> cShared_ptr< cAsset_Meta >;

		template< class Ty >
		requires std::is_base_of_v< cAsset, Ty >
		auto GetAssetsOfType( const int32_t _max_count = 0 ) -> std::vector< cShared_ptr< cAsset_Meta > >
		{
			return GetAssetsOfType( Ty::getStaticType(), _max_count );
		} // Get_Assets_Of_Type
		
		auto GetAssetsOfType( const iRuntimeClass& _class, int32_t _max_count = 0 ) -> std::vector< cShared_ptr< cAsset_Meta > >;

		template< class Ty >
		requires std::is_base_of_v< cAsset, Ty >
		auto GetRange()
		{
			return m_assets_.equal_range( Ty::getStaticType() );
		}
		void AddAsset   ( const cShared_ptr< cAsset_Meta >& _asset );
		void RemoveAsset( const cShared_ptr< cAsset_Meta >& _asset );

		[[ nodiscard ]] auto begin( void ) const { return asset_iterator( m_assets_.begin() ); }
		[[ nodiscard ]] auto end  ( void ) const { return asset_iterator( m_assets_.end  () ); }

		[[ nodiscard ]] bool empty( void ) const { return m_assets_.empty(); }

	protected:


	private:
		friend class sk::cAsset_Manager;

		asset_map_t         m_assets_;
		asset_counter_map_t m_counters_;
	};

} // sk::Assets::
