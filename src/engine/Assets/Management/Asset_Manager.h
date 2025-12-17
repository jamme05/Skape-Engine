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
	class cAsset_Meta;

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
		friend class cAsset_Meta;
	public:
		typedef std::pair< str_hash, std::string > file_pair_t;

		 cAsset_Manager( void );
		~cAsset_Manager( void ) override;

		auto getAsset( const cUUID _id ) -> cShared_ptr< cAsset_Meta >;

		template< class Ty >
		requires std::is_base_of_v< cAsset_Meta, Ty >
		auto getAssetAs( const cUUID _id ) -> cShared_ptr< Ty >
		{
			if( auto asset = getAsset( _id ); asset && asset->getClass().isDerivedFrom( Ty::GetStaticClass() ) )
				return asset.Cast< Ty >();

			return nullptr;
		} // getAsset

		template< class Ty, class... Args >
		requires std::is_base_of_v< cAsset_Meta, Ty >
		cShared_ptr< Ty > createAsset( Args... _args ) // TODO: Add &&
		{
			auto asset = Ty::create_shared( std::forward< Args >( _args )... );
			if( asset )
				registerAsset( asset );

			return asset;
		} // createAsset

		// Gets the first asset by name.
		auto getAssetByName ( const str_hash& _name_hash ) -> cShared_ptr< cAsset_Meta >;
		// Gets all assets by name.
		auto getAssetsByName( const str_hash& _name_hash ) -> Assets::cAsset_List;

		// Gets the first asset by path.
		auto getAssetByPath ( const str_hash& _path_hash ) -> cShared_ptr< cAsset_Meta >;
		// Gets all assets made from the same file. ( Multiple in the case of packs )
		auto getAssetsByPath( const str_hash& _path_hash ) -> Assets::cAsset_List;

		 /**
		  * 
		  * @param _asset  The asset to register.
		  * @param _reload Tells the asset manager if it should replace the previous version of the asset with this.
		  * @return The asset ID of said asset.
		  */
		cUUID registerAsset( const cShared_ptr< cAsset_Meta >& _asset, const bool _reload = false );

		 /**
		  * 
		  * @param _path      The path to the folder.
		  * @param _recursive 
		  * @param _reload 
		  * @return 
		  */
		auto loadFolderMeta( const std::filesystem::path& _path, const bool _recursive = true, const bool _reload = false ) -> Assets::cAsset_List;
		auto loadFileMeta  ( const std::filesystem::path& _path, const bool _reload = false ) -> Assets::cAsset_List;

		void RequestLoadAsset  ( const cAsset_Meta& _partial );
		void RequestUnloadAsset( const cAsset_Meta& _partial, bool _force = false );
		auto GetAssetRefByName ( const str_hash& _name_hash, const cShared_ptr< iClass >& _self, bool _load_asset = true )
			-> cAsset_Ref;
		auto GetAssetRefByPath ( const str_hash& _path_hash, const cShared_ptr< iClass >& _self, bool _load_asset = true )
			-> cAsset_Ref;
		auto GetAssetRef       ( const cAsset_Meta& _partial, const cShared_ptr< iClass >& _self, bool _load_asset = true )
			-> cAsset_Ref;

		static auto getAbsolutePath ( const std::filesystem::path& _path ) -> std::filesystem::path;
		static void makeAbsolutePath(       std::filesystem::path& _path );

		enum class eLoadTask : uint8_t
		{
			kLoadPartial,
			kLoadAsset
		};

		using load_file_func_t = std::function< Assets::cAsset_List( const cAsset_Meta&, eLoadTask ) >;

		void AddFileLoader( const std::vector< str_hash >& _extensions, const load_file_func_t& _function );
	
	private:
		typedef unordered_map< hash< cUUID >, cShared_ptr< cAsset_Meta > > id_to_asset_map_t;
		typedef multimap< str_hash, cShared_ptr< cAsset_Meta > >           str_to_asset_map_t;
		typedef unordered_map< str_hash, load_file_func_t >               extension_loader_map_t;
		typedef extension_loader_map_t::value_type                        extension_map_entry_t;
		typedef vector< cAsset_Worker >                                   loaders_vector_t;
		
		static auto loadGltfFile     ( const cAsset_Meta& _partial_asset, eLoadTask _load_task     ) -> Assets::cAsset_List;
		static auto handleGltfMesh   ( const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh       ) -> Assets::cAsset_List;
		static auto handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture ) -> cShared_ptr< cAsset_Meta >;

		static auto loadPngFile      ( const cAsset_Meta& _path, eLoadTask _load_task ) -> Assets::cAsset_List;

		void pushAssetLoadJob  ( const cShared_ptr< cAsset_Meta >& _meta, bool _reload );
		void pushAssetUnloadJob( const cShared_ptr< cAsset_Meta >& _meta );
		void pushNewListenerJob( const cShared_ptr< cAsset_Meta >& _meta, const cAsset_Meta::dispatcher_t::listener_t& _listener );
		
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
