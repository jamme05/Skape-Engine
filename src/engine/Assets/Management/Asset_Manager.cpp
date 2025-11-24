/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Manager.h"

#include <Assets/Asset_List.h>
#include <Assets/Management/Asset_Loader.h>
#include <Assets/Mesh.h>
#include <Assets/Model.h>
#include <Assets/Texture.h>

#include <Scene/Managers/CameraManager.h>

#include <fastgltf/tools.hpp>

namespace sk
{
	cAsset_Manager::cAsset_Manager( void )
	{
		loadEmbedded();
	} // cAssetManager

	cAsset_Manager::~cAsset_Manager( void )
	{
		m_assets.clear();
	} // ~cAssetManager

	auto cAsset_Manager::getAsset( const cUUID _id ) -> cShared_ptr< cPartialAsset >
	{
		if( const auto asset_itr = m_assets.find( _id ); asset_itr != m_assets.end() )
			return asset_itr->second;

		return nullptr;
	} // getAsset

	auto cAsset_Manager::getAssetByName( const str_hash& _name_hash ) -> cShared_ptr< cPartialAsset >
	{
		if( const auto itr = m_asset_name_map.find( _name_hash ); itr != m_asset_name_map.end() )
			return itr->second;

		return nullptr;
	} // getAssetByName
	auto cAsset_Manager::getAssetsByName( const str_hash& _name_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		const auto range = m_asset_name_map.equal_range( _name_hash );
		for( auto it = range.first; it != range.second; ++it )
			assets.add_asset( it->second );

		return assets;
	} // getAssetsByName

	auto cAsset_Manager::getAssetByPath( const str_hash& _path_hash ) -> cShared_ptr< cPartialAsset >
	{
		if( const auto itr = m_asset_path_map.find( _path_hash ); itr != m_asset_path_map.end() )
			return itr->second;

		return nullptr;
	} // getAssetByPath

	auto cAsset_Manager::getAssetsByPath( const str_hash& _path_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		const auto range = m_asset_path_map.equal_range( _path_hash );
		for( auto it = range.first; it != range.second; ++it )
			assets.add_asset( it->second );

		return assets;
	} // getAssetsByPath

	cUUID cAsset_Manager::registerAsset( const cShared_ptr< cPartialAsset >& _asset, const bool _reload )
	{
		// TODO: Add check if asset already exists.
		
		const auto id = GenerateRandomUUID();
		m_assets[ id ] = _asset;
		_asset->m_uuid_   = id;
		m_asset_name_map.insert( { _asset->GetName().hash(), _asset } );
		m_asset_path_map.insert( { _asset->GetPath().hash(), _asset } );

		return id;
	} // registerAsset

	auto cAsset_Manager::loadFolder( const std::filesystem::path& _path, const bool _recursive, const bool _reload ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;

		// TODO: Implament loading folders.
		if( _recursive )
		{
			std::filesystem::recursive_directory_iterator iter( _path );
			for( const auto& file : iter )
			{
				// Look into making it safe. https://en.cppreference.com/w/cpp/filesystem/directory_entry.html
				if( file.is_regular_file() )
					assets += loadFile( file.path(), _reload );
			}
		}
		else
		{
			std::filesystem::directory_entry iter( _path );
		}

		return assets;
	} // loadFolder

	auto cAsset_Manager::loadFile( const std::filesystem::path& _path, const bool _reload ) -> Assets::cAsset_List
	{
		const auto extension_hash = str_hash( _path.extension() );

		const auto callback_pair = m_load_callbacks.find( extension_hash );

		if( callback_pair == m_load_callbacks.end() )
			return {};

		const auto absolute_path = getAbsolutePath( _path );
		// TODO: Full path asset search. (aka find asset with path)
		const auto path_hash     = str_hash( absolute_path );

		auto assets = callback_pair->second( absolute_path );

		if( assets.empty() )
			return {};

		for( auto& asset : assets )
		{
			registerAsset( asset );
		}

		return assets;
	} // loadFile

	auto cAsset_Manager::getAbsolutePath( const std::filesystem::path& _path ) -> std::filesystem::path
	{
		return std::filesystem::path( SK_GAME_DIR ) /= _path;
	} // getAbsolutePath

	void cAsset_Manager::makeAbsolutePath( std::filesystem::path& _path )
	{
		_path = getAbsolutePath( _path );
	} // makeAbsolutePath

	void cAsset_Manager::AddFileLoader( const std::vector< str_hash >& _extensions, load_file_func_t _function )
	{
		for( const auto& extension : _extensions )
			m_load_callbacks.emplace( extension, _function );
	} // AddFileLoader

	auto cAsset_Manager::loadGltfFile( const std::filesystem::path& _path ) -> Assets::cAsset_List
	{
		fastgltf::Parser parser;

		auto data = fastgltf::GltfDataBuffer::FromPath( _path );
		if( data.error() != fastgltf::Error::None )
			return {};

		auto raw_asset = parser.loadGltf( data.get(), _path.parent_path(), fastgltf::Options::None );
		if( auto error = raw_asset.error(); error != fastgltf::Error::None )
			return {};

		auto& asset = raw_asset.get();

		Assets::cAsset_List assets;

		for( auto& node : asset.nodes )
		{
			// TODO: Models
		}
		for( auto& texture : asset.textures )
		{
			if( auto texture_asset = handleGltfTexture( asset, texture ) )
				assets.add_asset( texture_asset );
		}
		for( auto& mesh : asset.meshes )
		{
			auto mesh_assets = handleGltfModel( asset, mesh );
			assets += mesh_assets;
		}

		return assets;
	} // loadGltfFile

	auto cAsset_Manager::handleGltfModel( const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		auto model_asset = sk::make_shared< Assets::cModel >();
		for( auto& primitive : _mesh.primitives )
		{
			// Check if primitive has indices, go to next if it doesn't.
			if( !primitive.indicesAccessor.has_value() )
				continue;

			auto mesh_asset = sk::make_shared< Assets::cMesh >();
		} // auto& primitive : _mesh.primitives

		return assets;
	} // handleGltfMesh

	auto cAsset_Manager::handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture ) -> cShared_ptr< cPartialAsset >
	{
		if( !_texture.imageIndex.has_value() )
			return nullptr;

		auto& image = _asset.images[ _texture.imageIndex.value() ];

		const uint8_t* target_buffer = nullptr;
		size_t         size;
		fastgltf::MimeType type;

		// TODO: Support external sources.

		std::visit(fastgltf::visitor {
			[](auto& arg) {},
			[&](const fastgltf::sources::URI& uri) {},
			[&](fastgltf::sources::Array& vector) {
				size = vector.bytes.size();
				target_buffer = reinterpret_cast< const uint8_t* >( vector.bytes.data() );
				type = vector.mimeType;
			},
			[&](const fastgltf::sources::BufferView& view) {
				auto& bufferView = _asset.bufferViews[view.bufferViewIndex];
				auto& buffer = _asset.buffers[bufferView.bufferIndex];
				// Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
				// to just copy the buffer data again for the texture. Besides, this is just an example.

				type = view.mimeType;

				if( const auto vector = std::get_if< fastgltf::sources::Array >( &buffer.data ) )
				{
					size = vector->bytes.size();
					target_buffer = reinterpret_cast< const uint8_t* >( vector->bytes.data() ) + bufferView.byteOffset;
				}
			},
		}, image.data);

		if( target_buffer )
			return make_shared< Assets::cTexture >( image.name.c_str(), target_buffer, size );

		return nullptr;
	} // handleGltfTexture

	auto cAsset_Manager::loadPngFile( const std::filesystem::path& _path ) -> Assets::cAsset_List
	{
		// TODO: Standalone Png file loader.
		return {};
	} // loadPNGFile

	void cAsset_Manager::loadEmbedded( void )
	{
	} // loadEmbedded
} // sk::
