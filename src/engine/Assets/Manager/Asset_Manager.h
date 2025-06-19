/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */


#pragma once

#include "Assets/Asset.h"

#include "Containers/Map.h"

#include "Misc/Singleton.h"
#include "Misc/Smart_Ptrs.h"

#include "fastgltf/core.hpp"

namespace sk
{
	class iAsset;

	namespace Assets
	{
		class cAsset_List;

		enum class eGltfFilter
		{
			kScene,
			kModel,
			kTexture,
		};
	}  // Assets

	class cAssetManager : public cSingleton< cAssetManager >
	{
	public:
		 cAssetManager( void );
		~cAssetManager( void );

		auto getAsset( const uint64_t _id ) -> cShared_ptr< iAsset >;

		template< class Ty >
		requires std::is_base_of_v< iAsset, Ty >
		auto getAssetAs( const uint64_t _id ) -> cShared_ptr< Ty >
		{
			if( auto asset = getAsset( _id ); asset && asset->getClass().isDerivedFrom( Ty::GetStaticClass() ) )
				return asset.cast< Ty >();

			return nullptr;
		} // getAsset

		template< class Ty, class... Args >
		requires std::is_base_of_v< iAsset, Ty >
		cShared_ptr< Ty > createAsset( Args... _args ) // TODO: Add &&
		{
			auto asset = Ty::create_shared( std::forward< Args >( _args )... );
			if( asset )
				registerAsset( asset );

			return asset;
		} // createAsset

		// Gets the first asset by name.
		auto getAssetByName ( const str_hash& _name_hash ) -> cShared_ptr< iAsset >;
		// Gets all assets by name.
		auto getAssetsByName( const str_hash& _name_hash ) -> Assets::cAsset_List;

		// Gets the first asset by path.
		auto getAssetByPath ( const str_hash& _path_hash ) -> cShared_ptr< iAsset >;
		// Gets all assets made from the same file. ( Multiple in the case of packs )
		auto getAssetsByPath( const str_hash& _path_hash ) -> Assets::cAsset_List;

		 /**
		  * 
		  * @param _asset  The asset to register.
		  * @param _reload Tells the asset manager if it should replace the previous version of the asset with this.
		  * @return The asset ID of said asset.
		  */
		uint64_t registerAsset( const cShared_ptr< iAsset >& _asset, const bool _reload = false );

		 /**
		  * 
		  * @param _path      The path to the folder.
		  * @param _recursive 
		  * @param _reload 
		  * @return 
		  */
		auto loadFolder( const std::filesystem::path& _path, const bool _recursive = true, const bool _reload = false ) -> Assets::cAsset_List;
		auto loadFile  ( const std::filesystem::path& _path, const bool _reload = false ) -> Assets::cAsset_List;

		static auto getAbsolutePath ( const std::filesystem::path& _path ) -> std::filesystem::path;
		static void makeAbsolutePath(       std::filesystem::path& _path );

	private:
		typedef Assets::cAsset_List( *load_file_func_t )( const std::filesystem::path& );
		typedef unordered_map< hash< uint64_t >, cShared_ptr< iAsset > > id_to_asset_map_t;
		typedef multimap< str_hash, cShared_ptr< iAsset > >     name_to_asset_map_t;
		typedef unordered_map< str_hash, load_file_func_t >      extension_loader_map_t;
		typedef extension_loader_map_t::value_type extension_map_entry_t;

#define EXTENSION_ENTRY( Ext, Func ) extension_map_entry_t{ str_hash( Ext ), Func },

		static auto  loadGltfFile     ( const std::filesystem::path& _path ) -> Assets::cAsset_List;
		static auto  handleGltfModel  ( const fastgltf::Asset& _asset, fastgltf::Mesh&    _mesh    ) -> Assets::cAsset_List;
		static auto  handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture ) -> cShared_ptr< iAsset >;

		static auto  loadPngFile      ( const std::filesystem::path& _path ) -> Assets::cAsset_List;

		void loadEmbedded( void );

		// It may show an error but is perfectly fine.
		extension_loader_map_t m_load_callbacks
		{
			EXTENSION_ENTRY( ".glb",  loadGltfFile )
			EXTENSION_ENTRY( ".gltf", loadGltfFile )
			EXTENSION_ENTRY( ".png",  loadPngFile  )
		};

		id_to_asset_map_t   m_assets;
		name_to_asset_map_t m_asset_name_map;
		name_to_asset_map_t m_asset_path_map;

	};

	namespace Assets
	{
		template< class Ty, class... Args >
		cShared_ptr< Ty > create( Args... _args ){ auto asset = sk::cShared_ptr< Ty >( SK_NEW( Ty, 1, _args... ) ); cAssetManager::get().registerAsset( asset ); return asset; }
	} // Assets::

} // sk::
