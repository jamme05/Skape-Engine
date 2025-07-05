/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Manager.h"

#include "fastgltf/tools.hpp"

#include <Assets/Mesh.h>

#include <Assets/Asset_List.h>
#include "Assets/Model.h"
#include "Assets/Texture.h"
#include "Graphics/cRenderer.h"
#include "Helpers/Mesh_Helper.h"
#include "Platform/cPlatform.h"
#include "Scene/Managers/CameraManager.h"

namespace sk
{
	cAssetManager::cAssetManager( void )
	{
		loadEmbedded();
	} // cAssetManager

	cAssetManager::~cAssetManager( void )
	{
		m_assets.clear();
	} // ~cAssetManager

	auto cAssetManager::getAsset( const uint64_t _id ) -> cShared_ptr< iAsset >
	{
		if( const auto asset_itr = m_assets.find( _id ); asset_itr != m_assets.end() )
			return asset_itr->second;

		return nullptr;
	} // getAsset

	auto cAssetManager::getAssetByName( const str_hash& _name_hash ) -> cShared_ptr< iAsset >
	{
		if( const auto itr = m_asset_name_map.find( _name_hash ); itr != m_asset_name_map.end() )
			return itr->second;

		return nullptr;
	} // getAssetByName
	auto cAssetManager::getAssetsByName( const str_hash& _name_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		const auto range = m_asset_name_map.equal_range( _name_hash );
		for( auto it = range.first; it != range.second; ++it )
			assets.add_asset( it->second );

		return assets;
	} // getAssetsByName

	auto cAssetManager::getAssetByPath( const str_hash& _path_hash ) -> cShared_ptr< iAsset >
	{
		if( const auto itr = m_asset_path_map.find( _path_hash ); itr != m_asset_path_map.end() )
			return itr->second;

		return nullptr;
	} // getAssetByPath

	auto cAssetManager::getAssetsByPath( const str_hash& _path_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		const auto range = m_asset_path_map.equal_range( _path_hash );
		for( auto it = range.first; it != range.second; ++it )
			assets.add_asset( it->second );

		return assets;
	} // getAssetsByPath

	uint64_t cAssetManager::registerAsset( const cShared_ptr< iAsset >& _asset, const bool _reload )
	{
		// TODO: Add check if asset already exists.

		// TODO: Maybe make a new way to generate ids.
		const uint64_t id  = m_assets.size();
		m_assets[ id ]       = _asset;
		_asset->m_id         = id;
		m_asset_name_map.insert( { _asset->getNameHash(), _asset } );
		m_asset_path_map.insert( { _asset->getPath(),     _asset } );

		return id;
	} // registerAsset

	auto cAssetManager::loadFolder( const std::filesystem::path& _path, const bool _recursive, const bool _reload ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;

		if( _recursive )
		{
			std::filesystem::recursive_directory_iterator iter( _path );
			for( const auto& file : iter )
			{
				assets += loadFile( file.path(), _reload );
			}
		}
		else
		{
			std::filesystem::directory_entry iter( _path );
		}

		return assets;
	} // loadFolder

	auto cAssetManager::loadFile( const std::filesystem::path& _path, const bool _reload ) -> Assets::cAsset_List
	{
		const auto extension_hash = str_hash( _path.extension() );

		const auto callback_pair = m_load_callbacks.find( extension_hash );

		if( callback_pair == m_load_callbacks.end() )
			return {};

		const auto absolute_path = getAbsolutePath( _path );
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

	auto cAssetManager::getAbsolutePath( const std::filesystem::path& _path ) -> std::filesystem::path
	{
		return std::filesystem::path( SK_GAME_DIR ) /= _path;
	} // getAbsolutePath

	void cAssetManager::makeAbsolutePath( std::filesystem::path& _path )
	{
		_path = getAbsolutePath( _path );
	} // makeAbsolutePath

	void cAssetManager::AddFileLoader( const std::vector< str_hash >& _extensions, load_file_func_t _function )
	{
		for( const auto& extension : _extensions )
			m_load_callbacks.emplace( extension, _function );
	} // AddFileLoader

	auto cAssetManager::loadGltfFile( const std::filesystem::path& _path ) -> Assets::cAsset_List
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

	auto cAssetManager::handleGltfModel( const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		auto model_asset = Assets::cModel::create_shared();
		for( auto& primitive : _mesh.primitives )
		{
			// Check if primitive has indices, go to next if it doesn't.
			if( !primitive.indicesAccessor.has_value() )
				continue;

			auto mesh_asset = Assets::cMesh::create_shared( std::string( _mesh.name ) );
			if( Graphics::Helpers::cMesh_Helper::ParseGltfPrimitiveMesh( mesh_asset, _asset, primitive ) )
			{
				
			}
		} // auto& primitive : _mesh.primitives

		return assets;
	} // handleGltfMesh

	auto cAssetManager::handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture ) -> cShared_ptr< iAsset >
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
		{
			Assets::cTexture::eSourceType imageType = Assets::cTexture::eSourceType::kInvalid;
			switch( type )
			{
			case fastgltf::MimeType::PNG:  imageType = Assets::cTexture::eSourceType::kPNG; break;
			case fastgltf::MimeType::JPEG: imageType = Assets::cTexture::eSourceType::kJPG; break;
			}
			if( imageType == Assets::cTexture::eSourceType::kInvalid )
				return nullptr;

			return make_shared< Assets::cTexture >( image.name.c_str(), target_buffer, size, imageType );
		}

		return nullptr;
	}

	auto cAssetManager::loadPngFile( const std::filesystem::path& _path ) -> Assets::cAsset_List
	{
		// TODO: Standalone Png file loader.
		return {};
	} // loadPNGFile

	void cAssetManager::loadEmbedded( void )
	{
		constexpr auto resolution = cVector2u( 16 );
		constexpr auto byte_size  = static_cast< size_t >( resolution.x * resolution.y ) * 4ull; // x * y * RGBA
		std::vector< uint8_t > white_texture( byte_size, 0xff );

		Graphics::cRenderer::m_white_texture = createAsset< Assets::cTexture >( "White", white_texture.data(), resolution, Assets::cTexture::eFormat::kRGBA8, true );
	} // loadEmbedded
} // sk::
