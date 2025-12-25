/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Manager.h"

#include <fastgltf/tools.hpp>

#include <Assets/Asset_List.h>
#include <Assets/Asset_Ptr.h>
#include <Assets/Management/Asset_Loader.h>
#include <Assets/Mesh.h>
#include <Assets/Model.h>
#include <Assets/Texture.h>

#include <Graphics/Buffer/Dynamic_Buffer.h>

#include <Scene/Managers/CameraManager.h>

#include "Asset_Job_Manager.h"


namespace sk
{
	cAsset_Manager::cAsset_Manager()
	{
		loadEmbedded();

		Assets::Jobs::cAsset_Job_Manager::init();
	} // cAssetManager

	cAsset_Manager::~cAsset_Manager()
	{
		auto& job_manager = Assets::Jobs::cAsset_Job_Manager::get();
		for( auto& asset : m_assets_ | std::views::values )
			job_manager.
		Assets::Jobs::cAsset_Job_Manager::shutdown();
		
		m_assets_.clear();
	} // ~cAssetManager

	auto cAsset_Manager::getAsset( const cUUID _id ) -> cShared_ptr< cAsset_Meta >
	{
		if( const auto asset_itr = m_assets_.find( _id ); asset_itr != m_assets_.end() )
			return asset_itr->second;

		return nullptr;
	} // getAsset

	auto cAsset_Manager::getAssetByName( const str_hash& _name_hash ) -> cShared_ptr< cAsset_Meta >
	{
		if( const auto itr = m_asset_name_map_.find( _name_hash ); itr != m_asset_name_map_.end() )
			return itr->second;

		return nullptr;
	} // getAssetByName
	auto cAsset_Manager::getAssetsByName( const str_hash& _name_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		const auto range = m_asset_name_map_.equal_range( _name_hash );
		for( auto it = range.first; it != range.second; ++it )
			assets.addAsset( it->second );

		return assets;
	} // getAssetsByName

	auto cAsset_Manager::getAssetByPath( const str_hash& _path_hash ) -> cShared_ptr< cAsset_Meta >
	{
		if( const auto itr = m_asset_path_map_.find( _path_hash ); itr != m_asset_path_map_.end() )
			return itr->second;

		return nullptr;
	} // getAssetByPath

	auto cAsset_Manager::getAssetsByPath( const str_hash& _path_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		const auto [ fst, snd ] = m_asset_path_map_.equal_range( _path_hash );
		assets.m_assets.insert( fst, snd );

		return assets;
	} // getAssetsByPath

	cUUID cAsset_Manager::registerAsset( const cShared_ptr< cAsset_Meta >& _asset, const bool _reload )
	{
		// TODO: Add check if asset already exists.
		
		const auto id = GenerateRandomUUID();
		m_assets_[ id ] = _asset;
		_asset->m_uuid_   = id;
		m_asset_name_map_.insert( { _asset->GetName().hash(), _asset } );
		m_asset_path_map_.insert( { _asset->GetPath().hash(), _asset } );

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

		const auto callback_pair = m_load_callbacks_.find( extension_hash );

		if( callback_pair == m_load_callbacks_.end() )
			return {};

		const auto absolute_path = getAbsolutePath( _path );

		Assets::cAsset_List assets;
		callback_pair->second( _path, assets, eAssetTask::kLoadMeta );

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

	auto cAsset_Manager::GetAssetPtrByName( const str_hash& _name_hash, const cShared_ptr< iClass >& _self, bool _load_asset ) -> cAsset_Ptr
	{
		if( auto asset = getAssetByName( _name_hash ) )
			return GetAssetPtr( *asset, _self, _load_asset );
		return {};
	}

	auto cAsset_Manager::GetAssetPtrByPath( const str_hash& _path_hash, const cShared_ptr< iClass >& _self, bool _load_asset ) -> cAsset_Ptr
	{
		if( auto asset = getAssetByPath( _path_hash ) )
			return GetAssetPtr( *asset, _self, _load_asset );
		return {};
	}

	auto cAsset_Manager::GetAssetPtrById( const cUUID& _uuid, const cShared_ptr< iClass >& _self,
		bool _load_asset ) -> cAsset_Ptr
	{
		if( auto asset = getAsset( _uuid ) )
			return GetAssetPtr( *asset, _self, _load_asset );
		return {};
	}

	auto cAsset_Manager::GetAssetPtr( const cAsset_Meta& _partial, const cShared_ptr< iClass >& _self, bool _load_asset ) -> cAsset_Ptr
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
			m_load_callbacks_.emplace( extension, _function );
	} // AddFileLoader
	
	auto cAsset_Manager::GetFileLoader( const str_hash& _extension_hash ) -> load_file_func_t
	{
		if( const auto itr = m_load_callbacks_.find( _extension_hash ); itr != m_load_callbacks_.end() )
			return itr->second;
		return nullptr;
	}

	void cAsset_Manager::addPathReferrer( const str_hash& _path_hash, const void* _referrer )
	{
		auto& [ referrers ] = m_path_ref_map_[ _path_hash ];
		referrers.emplace( _referrer );
		
		auto  [ fst, lst ] = m_asset_path_map_.equal_range( _path_hash );

		// TODO: Remember what I was planning here.
	}

	void cAsset_Manager::removePathReferrer( const str_hash& _path_hash, const void* _referrer )
	{
		const auto itr = m_path_ref_map_.find( _path_hash );
		if( itr == m_path_ref_map_.end() )
			return;

		auto& referrers = itr->second.referrers;
		referrers.erase( referrers.find( _referrer ) );
	}

	void cAsset_Manager::loadGltfFile( const std::filesystem::path& _path, Assets::cAsset_List& _asset_metas, eAssetTask _load_task )
	{
		if( _load_task == eAssetTask::kUnloadAsset )
			return;

		// TODO: Fix this asset loader.

		fastgltf::Parser parser;

		auto data = fastgltf::GltfDataBuffer::FromPath( _path );
		if( data.error() != fastgltf::Error::None )
			return;

		auto raw_asset = parser.loadGltf( data.get(), _path, fastgltf::Options::None );
		if( auto error = raw_asset.error(); error != fastgltf::Error::None )
			return;

		auto& asset = raw_asset.get();

		Assets::cAsset_List assets;
		for( auto& node : asset.nodes )
		{
			// TODO: Models
		}
		for( auto& texture : asset.textures )
		{
			assets.addAsset( handleGltfTexture( asset, texture, _load_task ) );
		}
		auto [ mesh_fst, mesh_lst ] = _asset_metas.GetRange< Assets::cMesh >();
		std::vector< cShared_ptr< cAsset_Meta > > sorted_meshes{ static_cast< size_t >( std::distance( mesh_fst, mesh_lst ) ) };
		for( ; mesh_fst != mesh_lst; ++mesh_fst )
		{
			auto meta = mesh_fst->second;
			meta->m_runtime_metadata_.begin()->
			sorted_meshes[ meta->m_runtime_metadata_[ "gltf_index" ] ];
		}
		
		for( size_t i = 0; i < asset.meshes.size(); i++ )
		{
			auto& mesh = asset.meshes.at( i );
			
			if( _load_task == eAssetTask::kLoadMeta )
				assets.addAsset( loadGltfMeshMeta( mesh, i ) );
			else
			{
				
				loadGltfMesh( *mesh_fst->second, asset, mesh, _load_task );
			}
		}

		if( _load_task == eAssetTask::kLoadMeta )
		{
			for( auto& meta : assets )
				meta->m_flags_ |= cAsset_Meta::eFlags::kSharesPath;
		}
	} // loadGltfFile
	
	auto cAsset_Manager::loadGltfMeshMeta( const fastgltf::Mesh& _mesh, const size_t _index ) -> cShared_ptr< cAsset_Meta >
	{
		return sk::make_shared< cAsset_Meta >( std::string_view( _mesh.name ) );
	}

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

	void cAsset_Manager::loadGltfMesh( const cAsset_Meta& _meta, const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh, const eAssetTask _task )
	{
		auto mesh_asset = SK_SINGLE( Assets::cMesh, _meta.GetName().string() );
		
		for( auto& primitive : _mesh.primitives )
		{
			// Check if primitive has indices, go to next if it doesn't.
			if( !primitive.indicesAccessor.has_value() )
				continue;
			
			auto& accessor = _asset.accessors[ primitive.indicesAccessor.value() ];

			fill_index_buffer( *mesh_asset, _asset, accessor );

			fill_vertex_buffers( *mesh_asset, _asset, primitive.attributes.data(), primitive.attributes.size() );

			// TODO: Support multiple primitives
			break;
		} // auto& primitive : _mesh.primitives
	} // handleGltfMesh
	
	auto cAsset_Manager::handleGltfTexture( const fastgltf::Asset& _asset, fastgltf::Texture& _texture, eAssetTask _task ) -> cShared_ptr< cAsset_Meta >
	{
		if( !_texture.imageIndex.has_value() )
			return nullptr;

		auto meta = sk::make_shared< cAsset_Meta >( _texture.name );
		
		if( _task == eAssetTask::kLoadMeta )
			return meta;
		
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

	void cAsset_Manager::loadPngFile( const std::filesystem::path& _path, Assets::cAsset_List& _assets, eAssetTask _load_task )
	{
		// TODO: Standalone Png file loader.
	} // loadPNGFile
	
	void cAsset_Manager::requestAssetLoadJob( const cShared_ptr< cAsset_Meta >& _meta, void* _referrer, bool _reload )
	{
		
	}

	void cAsset_Manager::requestAssetUnloadJob( const cShared_ptr< cAsset_Meta >& _meta, void* _referrer )
	{
		
	}

	void cAsset_Manager::loadEmbedded( void )
	{
	} // loadEmbedded
} // sk::
