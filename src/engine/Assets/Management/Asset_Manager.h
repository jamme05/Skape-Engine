/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */


#pragma once

#include <Assets/Asset.h>

#include <Containers/Map.h>
#include <Containers/Vector.h>

#include <Misc/Singleton.h>
#include <Misc/Smart_Ptrs.h>

#include <fastgltf/core.hpp>

#include <Assets/Asset_List.h>

namespace sk
{
	struct sAsset_Loader;
}

namespace sk
{
	class cPartialAsset;

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

	class cAsset_Manager : public cSingleton< cAsset_Manager >
	{
	public:
		typedef std::pair< str_hash, std::string > file_pair_t;

		 cAsset_Manager( void );
		~cAsset_Manager( void );

		auto getAsset( const cUUID _id ) -> cShared_ptr< cPartialAsset >;

		template< class Ty >
		requires std::is_base_of_v< cPartialAsset, Ty >
		auto getAssetAs( const cUUID _id ) -> cShared_ptr< Ty >
		{
			if( auto asset = getAsset( _id ); asset && asset->getClass().isDerivedFrom( Ty::GetStaticClass() ) )
				return asset.cast< Ty >();

			return nullptr;
		} // getAsset

		template< class Ty, class... Args >
		requires std::is_base_of_v< cPartialAsset, Ty >
		cShared_ptr< Ty > createAsset( Args... _args ) // TODO: Add &&
		{
			auto asset = Ty::create_shared( std::forward< Args >( _args )... );
			if( asset )
				registerAsset( asset );

			return asset;
		} // createAsset

		// Gets the first asset by name.
		auto getAssetByName ( const str_hash& _name_hash ) -> cShared_ptr< cPartialAsset >;
		// Gets all assets by name.
		auto getAssetsByName( const str_hash& _name_hash ) -> Assets::cAsset_List;

		// Gets the first asset by path.
		auto getAssetByPath ( const str_hash& _path_hash ) -> cShared_ptr< cPartialAsset >;
		// Gets all assets made from the same file. ( Multiple in the case of packs )
		auto getAssetsByPath( const str_hash& _path_hash ) -> Assets::cAsset_List;

		 /**
		  * 
		  * @param _asset  The asset to register.
		  * @param _reload Tells the asset manager if it should replace the previous version of the asset with this.
		  * @return The asset ID of said asset.
		  */
		cUUID registerAsset( const cShared_ptr< cPartialAsset >& _asset, const bool _reload = false );

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

		using load_file_func_t = std::function< Assets::cAsset_List( const std::filesystem::path& _path ) >;

		void AddFileLoader( const std::vector< str_hash >& _extensions, load_file_func_t _function );

	private:
		typedef unordered_map< hash< cUUID >, cShared_ptr< cPartialAsset > > id_to_asset_map_t;
		typedef multimap< str_hash, cShared_ptr< cPartialAsset > >           str_to_asset_map_t;
		typedef unordered_map< str_hash, load_file_func_t >                  extension_loader_map_t;
		typedef extension_loader_map_t::value_type                           extension_map_entry_t;
		typedef vector< sAsset_Loader >                                      loaders_vector_t;


		static auto  loadGltfFile     ( const std::filesystem::path& _path ) -> Assets::cAsset_List;
		static auto  handleGltfModel  ( const fastgltf::Asset& _asset, fastgltf::Mesh&    _mesh    ) -> Assets::cAsset_List;
		static auto  handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture ) -> cShared_ptr< cPartialAsset >;

		static auto  loadPngFile      ( const std::filesystem::path& _path ) -> Assets::cAsset_List;

		void loadEmbedded( void );

#define EXTENSION_ENTRY( Ext, Func ) extension_map_entry_t{ str_hash( Ext ), Func },

		// It may show an error but is perfectly fine.
		extension_loader_map_t m_load_callbacks
		{
			EXTENSION_ENTRY( "glb",  loadGltfFile )
			EXTENSION_ENTRY( "gltf", loadGltfFile )
			EXTENSION_ENTRY( "png",  loadPngFile  )
		};
		// TODO: Make the asset loader multithreaded.
		loaders_vector_t   m_loaders;

		id_to_asset_map_t  m_assets;
		str_to_asset_map_t m_asset_name_map;
		str_to_asset_map_t m_asset_path_map;
	};

	namespace Assets
	{
		template< class Ty, class... Args >
		cShared_ptr< Ty > create( Args... _args ){ auto asset = sk::cShared_ptr< Ty >( SK_NEW( Ty, 1, _args... ) ); cAsset_Manager::get().registerAsset( asset ); return asset; }
	} // Assets::

} // sk::
