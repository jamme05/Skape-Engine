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

#include <Graphics/Buffer/Dynamic_Buffer.h>

#include "Assets/AssetRef.h"

namespace sk
{
	cAsset_Manager::cAsset_Manager()
	{
		loadEmbedded();

		const auto nr_of_loaders = std::max( 1u, std::thread::hardware_concurrency() / 4 );
		m_loaders.resize( nr_of_loaders );
		
		for( size_t i = 0; i < nr_of_loaders; ++i )
		{
			auto& worker = m_loaders[ i ];
			worker.m_thread_ = std::thread{ &cAsset_Worker::worker, &worker };
		}
		
	} // cAssetManager

	cAsset_Manager::~cAsset_Manager()
	{
		m_assets.clear();
	} // ~cAssetManager

	auto cAsset_Manager::getAsset( const cUUID _id ) -> cShared_ptr< cAsset_Meta >
	{
		if( const auto asset_itr = m_assets.find( _id ); asset_itr != m_assets.end() )
			return asset_itr->second;

		return nullptr;
	} // getAsset

	auto cAsset_Manager::getAssetByName( const str_hash& _name_hash ) -> cShared_ptr< cAsset_Meta >
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

	auto cAsset_Manager::getAssetByPath( const str_hash& _path_hash ) -> cShared_ptr< cAsset_Meta >
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

	cUUID cAsset_Manager::registerAsset( const cShared_ptr< cAsset_Meta >& _asset, const bool _reload )
	{
		// TODO: Add check if asset already exists.
		
		const auto id = GenerateRandomUUID();
		m_assets[ id ] = _asset;
		_asset->m_uuid_   = id;
		m_asset_name_map.insert( { _asset->GetName().hash(), _asset } );
		m_asset_path_map.insert( { _asset->GetPath().hash(), _asset } );

		return id;
	} // registerAsset

	auto cAsset_Manager::loadFolderMeta( const std::filesystem::path& _path, const bool _recursive, const bool _reload ) -> Assets::cAsset_List
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
					assets += loadFileMeta( file.path(), _reload );
			}
		}
		else
		{
			std::filesystem::directory_entry iter( _path );
		}

		return assets;
	} // loadFolder

	auto cAsset_Manager::loadFileMeta( const std::filesystem::path& _path, const bool _reload ) -> Assets::cAsset_List
	{
		const auto extension_hash = str_hash( _path.extension() );

		const auto callback_pair = m_load_callbacks.find( extension_hash );

		if( callback_pair == m_load_callbacks.end() )
			return {};

		const auto absolute_path = getAbsolutePath( _path );

		// As the functions require an asset to provide the path, we create a temporary here.
		auto temp_asset = cAsset_Meta{ "" };
		temp_asset.m_path_ = absolute_path.string();
		
		auto assets = callback_pair->second( temp_asset, eLoadTask::kLoadPartial );

		if( assets.empty() )
			return {};

		for( auto& asset : assets )
			registerAsset( asset );

		return assets;
	} // loadFile
	
	void cAsset_Manager::RequestLoadAsset( const cAsset_Meta& _partial )
	{
		
	}

	void cAsset_Manager::RequestUnloadAsset( const cAsset_Meta& _partial, bool _force )
	{
		
	}

	auto cAsset_Manager::GetAssetRefByName( const str_hash& _name_hash, const cShared_ptr< iClass >& _self, bool _load_asset ) -> cAsset_Ref
	{
		if( auto asset = getAssetByName( _name_hash ) )
			return GetAssetRef( *asset, _self, _load_asset );
		return {};
	}

	auto cAsset_Manager::GetAssetRefByPath( const str_hash& _path_hash, const cShared_ptr< iClass >& _self, bool _load_asset ) -> cAsset_Ref
	{
		if( auto asset = getAssetByPath( _path_hash ) )
			return GetAssetRef( *asset, _self, _load_asset );
		return {};
	}

	auto cAsset_Manager::GetAssetRef( const cAsset_Meta& _partial, const cShared_ptr< iClass >& _self, bool _load_asset ) -> cAsset_Ref
	{
		
	}

	auto cAsset_Manager::getAbsolutePath( const std::filesystem::path& _path ) -> std::filesystem::path
	{
		return std::filesystem::path( SK_GAME_DIR ) /= _path;
	} // getAbsolutePath

	void cAsset_Manager::makeAbsolutePath( std::filesystem::path& _path )
	{
		_path = getAbsolutePath( _path );
	} // makeAbsolutePath

	void cAsset_Manager::AddFileLoader( const std::vector< str_hash >& _extensions, const load_file_func_t& _function )
	{
		for( const auto& extension : _extensions )
			m_load_callbacks.emplace( extension, _function );
	} // AddFileLoader

	auto cAsset_Manager::loadGltfFile( const cAsset_Meta& _partial_asset, eLoadTask _load_task ) -> Assets::cAsset_List
	{
		fastgltf::Parser parser;

		auto path = std::filesystem::path{ _partial_asset.GetPath().string() };

		auto data = fastgltf::GltfDataBuffer::FromPath( path );
		if( data.error() != fastgltf::Error::None )
			return {};

		auto raw_asset = parser.loadGltf( data.get(), path.parent_path(), fastgltf::Options::None );
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
			auto mesh_assets = handleGltfMesh( asset, mesh );
			assets += mesh_assets;
		}

		return assets;
	} // loadGltfFile

	namespace 
	{
		auto get_accessor_source_bytes( const fastgltf::Asset& _asset, const fastgltf::Accessor& _accessor )
		{
			constexpr fastgltf::DefaultBufferDataAdapter adapter;
			
			return adapter( _asset, *_accessor.bufferViewIndex ).subspan( _accessor.byteOffset );
		}

		void fill_index_buffer( Assets::cMesh& _mesh, const fastgltf::Asset& _asset, const fastgltf::Accessor& _accessor )
		{
			auto& index_buffer = _mesh.GetIndexBuffer();
			
			switch( _accessor.componentType )
			{
			case fastgltf::ComponentType::UnsignedByte:
				_mesh.CreateIndexBufferFrom( Assets::cMesh::eIndexType::k16, nullptr, _accessor.count );
				fastgltf::copyFromAccessor< uint16_t >( _asset, _accessor, index_buffer->RawData() );
			break;
			case fastgltf::ComponentType::UnsignedShort:
			{
				const auto data = get_accessor_source_bytes( _asset, _accessor );
				_mesh.CreateIndexBufferFrom( Assets::cMesh::eIndexType::k16, data.data(), _accessor.count );
			}
			break;
			case fastgltf::ComponentType::UnsignedInt:
			{
				const auto data = get_accessor_source_bytes( _asset, _accessor );
				_mesh.CreateIndexBufferFrom( Assets::cMesh::eIndexType::k32, data.data(), _accessor.count );
			}
			break;
			default:
				SK_FATAL( "What the fuck" )
			}
		}

		void fill_vertex_buffer( Graphics::cDynamic_Buffer& _buffer, const fastgltf::Asset& _asset, const fastgltf::Accessor& _accessor )
		{
			const auto data = get_accessor_source_bytes( _asset, _accessor );

			memcpy( _buffer.RawData(), data.data(), data.size() );
		}

		template< size_t R, size_t C = 1 >
		type_info_t get_accessor_type( const fastgltf::ComponentType _type )
		{
			if constexpr( C > 1 )
			{
				// Matrices
				switch( _type )
				{
				case fastgltf::ComponentType::Invalid:       return nullptr;
				case fastgltf::ComponentType::Byte:          return &kTypeInfo< Math::cMatrix< R, C, int8_t > >;
				case fastgltf::ComponentType::UnsignedByte:  return &kTypeInfo< Math::cMatrix< R, C, uint8_t > >;
				case fastgltf::ComponentType::Short:         return &kTypeInfo< Math::cMatrix< R, C, int16_t > >;
				case fastgltf::ComponentType::UnsignedShort: return &kTypeInfo< Math::cMatrix< R, C, uint16_t > >;
				case fastgltf::ComponentType::Int:           return &kTypeInfo< Math::cMatrix< R, C, int32_t > >;
				case fastgltf::ComponentType::UnsignedInt:   return &kTypeInfo< Math::cMatrix< R, C, uint32_t > >;
				case fastgltf::ComponentType::Float:         return &kTypeInfo< Math::cMatrix< R, C, float > >;
				case fastgltf::ComponentType::Double:        return &kTypeInfo< Math::cMatrix< R, C, double > >;
				}
			}
			else if constexpr( R == 1 )
			{
				// Scalar
				switch( _type )
				{
				case fastgltf::ComponentType::Invalid:       return nullptr;
				case fastgltf::ComponentType::Byte:          return &kTypeInfo< int8_t >;
				case fastgltf::ComponentType::UnsignedByte:  return &kTypeInfo< uint8_t >;
				case fastgltf::ComponentType::Short:         return &kTypeInfo< int16_t >;
				case fastgltf::ComponentType::UnsignedShort: return &kTypeInfo< uint16_t >;
				case fastgltf::ComponentType::Int:           return &kTypeInfo< int32_t >;
				case fastgltf::ComponentType::UnsignedInt:   return &kTypeInfo< uint32_t >;
				case fastgltf::ComponentType::Float:         return &kTypeInfo< float >;
				case fastgltf::ComponentType::Double:        return &kTypeInfo< double >;
				}
			}
			else
			{
				// Vectors
				switch( _type )
				{
				case fastgltf::ComponentType::Invalid:       return nullptr;
				case fastgltf::ComponentType::Byte:          return &kTypeInfo< Math::cVector< R, int8_t > >;
				case fastgltf::ComponentType::UnsignedByte:  return &kTypeInfo< Math::cVector< R, uint8_t > >;
				case fastgltf::ComponentType::Short:         return &kTypeInfo< Math::cVector< R, int16_t > >;
				case fastgltf::ComponentType::UnsignedShort: return &kTypeInfo< Math::cVector< R, uint16_t > >;
				case fastgltf::ComponentType::Int:           return &kTypeInfo< Math::cVector< R, int32_t > >;
				case fastgltf::ComponentType::UnsignedInt:   return &kTypeInfo< Math::cVector< R, uint32_t > >;
				case fastgltf::ComponentType::Float:         return &kTypeInfo< Math::cVector< R, float > >;
				case fastgltf::ComponentType::Double:        return &kTypeInfo< Math::cVector< R, double > >;
				}
			}

			return nullptr;
		}

		type_info_t get_accessor_type( const fastgltf::Accessor& _accessor )
		{
			switch( _accessor.type )
			{
			case fastgltf::AccessorType::Invalid: return nullptr;
			case fastgltf::AccessorType::Scalar:  return get_accessor_type< 1 >( _accessor.componentType );
			case fastgltf::AccessorType::Vec2:    return get_accessor_type< 2 >( _accessor.componentType );
			case fastgltf::AccessorType::Vec3:    return get_accessor_type< 3 >( _accessor.componentType );
			case fastgltf::AccessorType::Vec4:    return get_accessor_type< 4 >( _accessor.componentType );
			case fastgltf::AccessorType::Mat2:    return get_accessor_type< 2, 2 >( _accessor.componentType );
			case fastgltf::AccessorType::Mat3:    return get_accessor_type< 3, 3 >( _accessor.componentType );
			case fastgltf::AccessorType::Mat4:    return get_accessor_type< 4, 4 >( _accessor.componentType );
			}

			return nullptr;
		}

		void fill_vertex_buffers( Assets::cMesh& _mesh, const fastgltf::Asset& _asset, const fastgltf::Attribute* _attributes, const size_t _attribute_count )
		{
			auto& vertex_buffers = _mesh.GetVertexBuffers();
			
			for( size_t i = 0; i < _attribute_count; i++ )
			{
				const auto& [ name, accessorIndex ] = _attributes[ i ];

				auto& accessor = _asset.accessors[ accessorIndex ];

				auto buffer = sk::make_shared< Graphics::cDynamic_Buffer >( std::format( "{}: {}", _mesh.GetName(), std::string_view{ name } ),
					Graphics::Buffer::eType::kVertex, accessor.normalized );

				buffer->AlignAs( get_accessor_type( accessor ), false );
				buffer->Resize( accessor.count );
				
				fill_vertex_buffer( *buffer, _asset, accessor );

				vertex_buffers.emplace( str_hash{ name }, buffer );
			}
		}
	} // ::

	auto cAsset_Manager::handleGltfMesh( const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		auto model_asset = sk::make_shared< Assets::cModel >();
		
		for( auto& primitive : _mesh.primitives )
		{
			// Check if primitive has indices, go to next if it doesn't.
			if( !primitive.indicesAccessor.has_value() )
				continue;

			auto mesh_asset = sk::make_shared< Assets::cMesh >();
			
			auto& accessor = _asset.accessors[ primitive.indicesAccessor.value() ];

			fill_index_buffer( *mesh_asset, _asset, accessor );

			fill_vertex_buffers( *mesh_asset, _asset, primitive.attributes.data(), primitive.attributes.size() );
			
		} // auto& primitive : _mesh.primitives

		return assets;
	} // handleGltfMesh

	auto cAsset_Manager::handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture ) -> cShared_ptr< cAsset_Meta >
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

	auto cAsset_Manager::loadPngFile( const cAsset_Meta& _path, eLoadTask _load_task ) -> Assets::cAsset_List
	{
		// TODO: Standalone Png file loader.
		return {};
	} // loadPNGFile

	void cAsset_Manager::loadEmbedded( void )
	{
	} // loadEmbedded
} // sk::
