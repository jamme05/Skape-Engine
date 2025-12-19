/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */


#pragma once

#include <fastgltf/core.hpp>

#include <Assets/Asset.h>
#include <Assets/Asset_List.h>

#include <Containers/Map.h>
#include <Containers/Vector.h>

#include <Misc/Singleton.h>
#include <Misc/Smart_Ptrs.h>


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
		auto loadFolder( const std::filesystem::path& _path, const bool _recursive = true, const bool _reload = false ) -> Assets::cAsset_List;
		auto loadFile  ( const std::filesystem::path& _path, const bool _reload = false ) -> Assets::cAsset_List;

		void RequestLoadAsset  ( const cAsset_Meta& _partial );
		void RequestUnloadAsset( const cAsset_Meta& _partial, bool _force = false );
		auto GetAssetPtrByName ( const str_hash& _name_hash, const cShared_ptr< iClass >& _self, bool _load_asset = true )
			-> cAsset_Ptr;
		auto GetAssetPtrByPath ( const str_hash& _path_hash, const cShared_ptr< iClass >& _self, bool _load_asset = true )
			-> cAsset_Ptr;
		auto GetAssetPtr       ( const cAsset_Meta& _partial, const cShared_ptr< iClass >& _self, bool _load_asset = true )
			-> cAsset_Ptr;

		static auto getAbsolutePath ( const std::filesystem::path& _path ) -> std::filesystem::path;
		static void makeAbsolutePath(       std::filesystem::path& _path );

		enum class eAssetTask : uint8_t
		{
			kLoadMeta,
			kLoadAsset,
			kRefreshAsset,
			kUnloadAsset,
		};

		using load_file_func_t = std::function< void( const std::filesystem::path&, Assets::cAsset_List&, eAssetTask ) >;

		void AddFileLoader( const std::vector< str_hash >& _extensions, const load_file_func_t& _function );
	
	private:
		struct sRef_Info
		{
			using referrer_set_t = std::unordered_multiset< const void* >;
			referrer_set_t referrers;
			
		};

		using id_to_asset_map_t      = unordered_map< hash< cUUID >, cShared_ptr< cAsset_Meta > >;
		using str_to_asset_map_t     = unordered_multimap< str_hash, cShared_ptr< cAsset_Meta > >;
		using path_to_ref_map_t      = unordered_map< str_hash, sRef_Info >;
		using extension_loader_map_t = unordered_map< str_hash, load_file_func_t >;
		using extension_map_entry_t  = extension_loader_map_t::value_type;

		void addPathReferrer   ( const str_hash& _path_hash, const void* _referrer );
		void removePathReferrer( const str_hash& _path_hash, const void* _referrer );
		
		static void loadGltfFile     ( const std::filesystem::path& _path, Assets::cAsset_List& _asset_metas, eAssetTask _load_task );
		static auto loadGltfMeshMeta ( const fastgltf::Mesh& _mesh, size_t _index ) -> cShared_ptr< cAsset_Meta >;
		static auto loadGltfMesh     ( const cAsset_Meta& _meta, const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh,       eAssetTask _task ) -> cShared_ptr< cAsset_Meta >;
		static auto handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture, eAssetTask _task ) -> cShared_ptr< cAsset_Meta >;

		static void loadPngFile      ( const std::filesystem::path& _path, Assets::cAsset_List& _assets, eAssetTask _load_task );

		void requestAssetLoadJob  ( const cShared_ptr< cAsset_Meta >& _meta, void* _referrer, bool _reload );
		void requestAssetUnloadJob( const cShared_ptr< cAsset_Meta >& _meta, void* _referrer );
		
		void loadEmbedded( void );

#define EXTENSION_ENTRY( Ext, Func ) extension_map_entry_t{ str_hash( Ext ), Func },

		// It may show an error but is perfectly fine.
		extension_loader_map_t m_load_callbacks_
		{
			EXTENSION_ENTRY( "glb",  loadGltfFile )
			EXTENSION_ENTRY( "gltf", loadGltfFile )
			EXTENSION_ENTRY( "png",  loadPngFile  )
		};

		id_to_asset_map_t  m_assets_;
		str_to_asset_map_t m_asset_name_map_;
		str_to_asset_map_t m_asset_path_map_;
		path_to_ref_map_t  m_path_ref_map_;
	};

	namespace Assets
	{
		template< class Ty, class... Args >
		cShared_ptr< Ty > create( Args... _args ){ auto asset = sk::cShared_ptr< Ty >( SK_NEW( Ty, 1, _args... ) ); cAsset_Manager::get().registerAsset( asset ); return asset; }
	} // Assets::

} // sk::
