/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Asset_Manager.h"

#include <any>

#include <fastgltf/tools.hpp>

#include <Assets/Management/Asset_Job_Manager.h>
#include <Assets/Mesh.h>
#include <Assets/Model.h>
#include <Assets/Texture.h>
#include <Assets/Utils/Asset_List.h>
#include <Assets/Access/Asset_Ptr.h>
#include <Graphics/Buffer/Dynamic_Buffer.h>
#include <Scene/Managers/CameraManager.h>



namespace sk
{
	cAsset_Manager::cAsset_Manager()
	{
		// TODO: Add some future way that uses a platform dependant function to get the executable path.
		// We're expected to be inside [Project Root]/Build/Project/startup
		// And we need to move to [Project Root]/game
		if( auto current_path = std::filesystem::current_path(); current_path.filename() == "startup" )
		{
			current_path = current_path.parent_path().parent_path().parent_path();
			std::filesystem::current_path( current_path / "game" );
		}
		else if( current_path.parent_path().filename() == "game" )
		{
			// We may otherwise be in [Project Root]/game/bin
			// In that case we only need to go down one directory.
			std::filesystem::current_path( current_path.parent_path() );
		}

		loadEmbedded();

		Assets::Jobs::cAsset_Job_Manager::init();
	} // cAssetManager

	cAsset_Manager::~cAsset_Manager()
	{
		auto& job_manager = Assets::Jobs::cAsset_Job_Manager::get();
		
		job_manager.m_shutting_down_.store( true );
		Assets::Jobs::sTask task;
		task.type = Assets::Jobs::eJobType::kUnload;
		
		std::unordered_set< str_hash > queued;
		for( auto& path_hash : m_asset_path_map_ | std::views::keys )
		{
			if( queued.contains( path_hash ) )
				continue;
			
			queued.insert( path_hash );
			
			auto assets = GetAssetsByPrecisePath( path_hash );
			
			// It's ugly but it works
			// TODO: Use stringID instead of str_hash in places like these.
			const auto path   = assets.begin()->GetAbsolutePath();
			const auto loader = GetFileLoader( assets.begin()->m_ext_ );
			
			if( loader == nullptr )
			{
				for( auto& asset : assets )
					asset->setAsset( nullptr );
				continue;
			}
			
			task.data = ::new( Memory::alloc_fast( sizeof( Assets::Jobs::sAssetTask ) ) ) Assets::Jobs::sAssetTask{
				.path    = path.view(), 
				.affected_assets = assets,
				.loader       = loader,
			};
			
			job_manager.push_task( task );
		}
		Assets::Jobs::cAsset_Job_Manager::shutdown();
		
		m_assets_.clear();
		m_asset_path_map_.clear();
		m_asset_name_map_.clear();
		m_path_ref_map_.clear();
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
			assets.AddAsset( it->second );

		return assets;
	} // getAssetsByName

	auto cAsset_Manager::GetAssetByPath( const std::string_view& _path ) -> cShared_ptr< cAsset_Meta >
	{
		if( _path[ 0 ] != '.' )
		{
			std::string new_path( _path.size() + 2, 0 );
			
			if( _path[ 1 ] == '/' )
				new_path = '.' + std::string{ _path };
			else
				new_path = "./" + std::string{ _path };
			
			return getAssetByPrecisePath( new_path );
		}
		
		return getAssetByPrecisePath( _path );
	} // getAssetByPath

	auto cAsset_Manager::getAssetsByPath( const std::string_view& _path ) -> Assets::cAsset_List
	{
		if( _path[ 0 ] != '.' )
		{
			std::string new_path( _path.size() + 2, 0 );
			
			if( _path[ 1 ] != '/' )
				new_path = '.' + std::string{ _path };
			else
				new_path = "./" + std::string{ _path };
			
			return GetAssetsByPrecisePath( new_path );
		}
		
		return GetAssetsByPrecisePath( _path );
	} // getAssetsByPath
	
	auto cAsset_Manager::getAssetByPrecisePath( const str_hash& _path_hash ) -> cShared_ptr<cAsset_Meta>
	{
		if( const auto itr = m_asset_path_map_.find( _path_hash ); itr != m_asset_path_map_.end() )
			return itr->second;

		return nullptr;
	}

	auto cAsset_Manager::GetAssetsByPrecisePath( const str_hash& _path_hash ) -> Assets::cAsset_List
	{
		Assets::cAsset_List assets;
		for( auto [ fst, lst ] = m_asset_path_map_.equal_range( _path_hash ); fst != lst; ++fst )
			assets.AddAsset( fst->second );

		return assets;
	}

	cUUID cAsset_Manager::registerAsset( const cShared_ptr< cAsset_Meta >& _asset, const bool _reload )
	{
		// TODO: Add check if asset already exists.
		if( _asset->m_uuid_ == cUUID::kInvalid )
		{
			// New asset, provide an uuid to it and register it.
			const auto id = GenerateRandomUUID();
			m_assets_[ id ] = _asset;
			_asset->m_uuid_   = id;
			m_asset_name_map_.insert( { _asset->GetName().hash(),  _asset } );
			m_asset_path_map_.insert( { _asset->GetAbsolutePath(), _asset } );
		}
		else if( _reload )
			_asset->Reload();

		return _asset->m_uuid_;
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
		auto ext = _path.extension().string();
		SK_ERR_IF( ext.empty(),
			"Error: File extension is empty." )
		
		ext = ext.substr( 1 );
		
		const auto extension = cStringID( ext );

		const auto callback_pair = m_load_callbacks_.find( extension );

		if( callback_pair == m_load_callbacks_.end() )
			return {};

		const auto absolute_path = getAbsolutePath( _path );

		Assets::cAsset_List assets;
		callback_pair->second( absolute_path, assets, Assets::eAssetTask::kLoadMeta );

		for( auto& asset : assets )
		{
			asset->setPath( absolute_path );
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

	void cAsset_Manager::AddFileLoaderForExtensions( const std::vector< cStringID >& _extensions, const load_file_func_t& _function )
	{
		for( const auto& extension : _extensions )
			AddFileLoaderForExtension( extension, _function );
	} // AddFileLoader
	
	void cAsset_Manager::AddFileLoaderForExtension( const cStringID& _extension, const load_file_func_t& _function )
	{
		SK_WARN_IF_RET( sk::Severity::kEngine, m_load_callbacks_.contains( _extension ),
			TEXT( "Warning: Asset manager already contains a loader for {}", _extension.view() ) )
		
		m_load_callbacks_.emplace( _extension, _function );
	}

	void cAsset_Manager::RemoveFileLoaders( const std::vector< cStringID >& _extensions )
	{
		for( const auto& extension : _extensions )
			m_load_callbacks_.erase( extension );
	}

	auto cAsset_Manager::GetFileLoader( const str_hash& _extension_hash ) -> load_file_func_t
	{
		if( const auto itr = m_load_callbacks_.find( cStringID::FromHash( _extension_hash ) ); itr != m_load_callbacks_.end() )
			return itr->second;
		return nullptr;
	}

	auto cAsset_Manager::GetExtensions() -> std::vector< cStringID >
	{
		static std::vector< cStringID > extensions;
		extensions.clear();
		extensions.reserve( m_load_callbacks_.size() );
		
		for( auto& extension : m_load_callbacks_ | std::views::keys )
			extensions.emplace_back( extension );
		
		return extensions;
	}

	void cAsset_Manager::addPathReferrer( const str_hash& _path_hash, const void* _referrer )
	{
		auto& [ referrers ] = m_path_ref_map_[ _path_hash ];
		referrers.emplace( _referrer );
		
		auto  [ fst, lst ] = m_asset_path_map_.equal_range( _path_hash );

		// TODO: Remember what I was planning here.
	}

	bool cAsset_Manager::removePathReferrer( const str_hash& _path_hash, const void* _referrer )
	{
		const auto itr = m_path_ref_map_.find( _path_hash );
		if( itr == m_path_ref_map_.end() )
			return false;

		auto& referrers = itr->second.referrers;
		referrers.erase( referrers.find( _referrer ) );
		
		return referrers.empty();
	}

	void cAsset_Manager::loadGltfFile( const std::filesystem::path& _path, Assets::cAsset_List& _metas, Assets::eAssetTask _load_task )
	{
		if( _load_task == Assets::eAssetTask::kUnloadAsset )
			return;
		
		// TODO: Cache the result from this gltf file for each individual asset.

		fastgltf::Parser parser;

		auto data = fastgltf::GltfDataBuffer::FromPath( _path );
		if( data.error() != fastgltf::Error::None )
			return;

		auto raw_asset = parser.loadGltf( data.get(), _path, fastgltf::Options::GenerateMeshIndices );
		if( auto error = raw_asset.error(); error != fastgltf::Error::None )
			return;

		auto& asset = raw_asset.get();

		for( auto& node : asset.nodes )
		{
			// TODO: Models
			// TODO: Prefabs?
			// TODO: Decide if models are gonna be a thing, or just have them be prefabs.
		}
		
		auto [ tex_fst, tex_lst ] = _metas.GetRange< Assets::cTexture >();
		for( size_t i = 0; i < asset.textures.size(); i++ )
		{
			if( _load_task == Assets::eAssetTask::kLoadMeta )
			{
				auto& texture = asset.textures[ i ];
				_metas.AddAsset( createGltfTextureMeta( texture, i ) );
			}
			else
			{
				auto& meta    = *tex_fst->second;
				auto& texture = asset.textures[ std::any_cast< size_t >( meta.m_info_[ "gltf_index" ] ) ];
				handleGltfTexture( meta, asset, texture, _load_task );
				++tex_fst;
			}
		}
		
		auto [ mesh_fst, mesh_lst ] = _metas.GetRange< Assets::cMesh >();
		for( size_t i = 0; i < asset.meshes.size(); i++ )
		{
			if( _load_task == Assets::eAssetTask::kLoadMeta )
			{
				auto& mesh = asset.meshes[ i ];
				_metas.AddAsset( createGltfMeshMeta( mesh, i ) );
			}
			else
			{
				auto& meta = *mesh_fst->second;
				auto& mesh = asset.meshes[ std::any_cast< size_t >( meta.m_info_[ "gltf_index" ] ) ];
				handleGltfMesh( meta, asset, mesh, _load_task );
				++mesh_fst;
			}
		}

		if( _load_task == Assets::eAssetTask::kLoadMeta )
		{
			for( auto& meta : _metas )
				meta->m_flags_ |= cAsset_Meta::eFlags::kSharesPath;
		}
	} // loadGltfFile
	
	auto cAsset_Manager::createGltfMeshMeta( const fastgltf::Mesh& _mesh, const size_t _index ) -> cShared_ptr< cAsset_Meta >
	{
		auto meta = sk::make_shared< cAsset_Meta >( std::string_view( _mesh.name ), &sk::kTypeInfo< Assets::cMesh > );
		
		meta->m_info_[ "gltf_index" ] = _index;
		
		return meta;
	}

	auto cAsset_Manager::createGltfTextureMeta( const fastgltf::Texture& _texture,
		size_t _index )->cShared_ptr< cAsset_Meta >
	{
		auto meta = sk::make_shared< cAsset_Meta >( std::string_view( _texture.name ), &sk::kTypeInfo< Assets::cTexture > );
		
		meta->m_info_[ "gltf_index" ] = _index;
		
		return meta;
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
				const auto& [ pmr_name, accessorIndex ] = _attributes[ i ];

				auto& accessor = _asset.accessors[ accessorIndex ];

				auto buffer = sk::make_shared< Graphics::cDynamic_Buffer >(
					std::format( "{}: {}", _mesh.GetName(), std::string_view{ pmr_name } ),
					Graphics::Buffer::eType::kVertex, accessor.normalized
				);

				cStringID name = std::string_view{ pmr_name }; 

				buffer->AlignAs( get_accessor_type( accessor ), false );
				buffer->Resize( accessor.count );
				
				fill_vertex_buffer( *buffer, _asset, accessor );

				vertex_buffers.emplace( name, buffer );

				switch( name.hash() )
				{
				case str_hash( "POSITION" ):
				{
					static constexpr cStringID kAliases[] = { "Position", "aPosition" };
					for( auto& alias : kAliases )
						vertex_buffers.emplace( alias, buffer );
				}
				break;
				case str_hash( "NORMAL" ):
				{
					static constexpr cStringID kAliases[] = { "Normal", "aNormal" };
					for( auto& alias : kAliases )
						vertex_buffers.emplace( alias, buffer );
				}
				break;
				case str_hash( "TEXCOORD_0" ):
				{
					static constexpr cStringID kAliases[] = { "TexCoord", "aTexCoord", "UV", "aUV" };
					for( auto& alias : kAliases )
						vertex_buffers.emplace( alias, buffer );
				}
				break;
				default: break;
				}
			}
		}
	} // ::

	void cAsset_Manager::handleGltfMesh( cAsset_Meta& _meta, const fastgltf::Asset& _asset, fastgltf::Mesh& _mesh, const Assets::eAssetTask _task )
	{
		const auto mesh_asset = sk::Memory::alloc< Assets::cMesh >( 1, std::source_location::current() , _meta.GetName().string() );
		
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
		
		_meta.setAsset( mesh_asset );
	} // handleGltfMesh
	
	void cAsset_Manager::handleGltfTexture( cAsset_Meta& _meta, const fastgltf::Asset& _asset, fastgltf::Texture& _texture, const Assets::eAssetTask _task )
	{
		if( !_texture.imageIndex.has_value() )
			return;
		
		if( _task == Assets::eAssetTask::kLoadMeta )
			return;
		
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
		
		_meta.setAsset( SK_SINGLE( Assets::cTexture, std::string{ image.name }, target_buffer, size ) );
	} // handleGltfTexture

	void cAsset_Manager::loadPngFile( const std::filesystem::path& _path, Assets::cAsset_List& _assets, Assets::eAssetTask _load_task )
	{
		// TODO: Standalone Png file loader.
	} // loadPNGFile

	void cAsset_Manager::loadEmbedded( void )
	{
	} // loadEmbedded
} // sk::
