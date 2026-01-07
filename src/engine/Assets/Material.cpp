//
//
// COPYRIGHT William Ask S. Ness 2025
//
//

#include "Material.h"

#include "Assets/Texture.h"
#include "Graphics/Utils/Shader_Link.h"
#include "Graphics/Utils/Shader_Reflection.h"

using namespace sk::Assets;

cMaterial::cBlock::cBlock( const cMaterial& _owner, std::string _name, const block_t* _info, const size_t _binding )
: m_pretty_name_( std::move( _name ) )
, m_info_( _info )
, m_binding_( _binding )
, m_buffer_( m_pretty_name_ + ": Constant buffer", _info->size, _info->size, Graphics::Buffer::eType::kConstant, false, false )
, m_owner_( &_owner )
{}

bool cMaterial::cBlock::SetUniformRaw( const cStringID& _name, const void* _data, size_t _size )
{
    const auto itr = m_uniform_map_.find( _name );
    
    SK_BREAK_RET_IF( sk::Severity::kEngine, itr == m_uniform_map_.end(),
        TEXT( "Warning: Unable to find uniform with the name, {}", _name.view() ), false )
    
    auto& uniform = itr->second;
    
    if( uniform.byte_size < _size )
    {
        SK_BREAK;
        SK_WARNING( sk::Severity::kGraphics,
            "Warning: Buffer size provided is greater than what this uniform ( \"{}\" ) supports. Provided: {}, Uniform size: {}",
            uniform.pretty_name, _size, uniform.byte_size )
        _size = uniform.byte_size;
    }
    
    std::visit( [ & ]( auto& _uniform_ptr )
    {
        std::memcpy( static_cast< void* >( _uniform_ptr ), _data, _size );
    }, uniform.accessor );
    
    return true;
}

cMaterial::cMaterial( Graphics::Utils::cShader_Link&& _shader_link )
: m_shader_link_( std::move( _shader_link ) )
{
    SK_ERR_IFN( m_shader_link_.IsValid(),
        "Error: Invalid shader link provided." )
    
    // TODO: Get rid of the wait.
    // TODO: Have some way to either update the material during the runtime and check the status
    // Or some event on when the link has refreshed.
    m_shader_link_.Complete();
    
    create_data();
}

void cMaterial::Complete()
{
    m_shader_link_.Complete();
}

auto cMaterial::GetBlock( const cStringID& _name ) -> cBlock*
{
    const auto itr = m_block_map_.find( _name );
    
    SK_BREAK_RET_IF( sk::Severity::kEngine, itr == m_block_map_.end(),
        TEXT( "Warning: Unable to find uniform with the name, {}", _name.view() ), nullptr )
    
    return &itr->second;
}

void cMaterial::SetTexture( const cStringID& _name, std::nullptr_t )
{
    const auto itr = m_sampler_map_.find( _name );
    
    SK_BREAK_IF( sk::Severity::kEngine, itr == m_sampler_map_.end(),
        TEXT( "Warning: Unable to find texture binding with the name, {}", _name.view() ) )

    const auto& sampler = itr->second;

    sampler->texture = sInvalid{};
}

void cMaterial::SetTexture( const cStringID& _name, const cShared_ptr< Graphics::Rendering::cRender_Target >& _texture )
{
    const auto itr = m_sampler_map_.find( _name );
    
    SK_BREAK_IF( sk::Severity::kEngine, itr == m_sampler_map_.end(),
        TEXT( "Warning: Unable to find texture binding with the name, {}", _name.view() ) )

    const auto& sampler = itr->second;
    auto& target = sampler->texture;
    if( _texture != nullptr )
        target = _texture;
    else
        target = sInvalid{};
}

void cMaterial::SetTexture( const cStringID& _name, const cShared_ptr< cAsset_Meta >& _texture_meta )
{
    const auto itr = m_sampler_map_.find( _name );
    
    // We need to do this due to the standard library acting weird when we compare it iterators inside a lambda.
    const bool is_failure = itr == m_sampler_map_.end();
    SK_BREAK_IF( sk::Severity::kEngine, is_failure,
        TEXT( "Warning: Unable to find texture binding with the name, {}", _name.view() ) )

    const auto& sampler = itr->second;
    auto& target = sampler->texture;
    if( _texture_meta != nullptr )
        target = cAsset_Ref< cTexture >{ nullptr, _texture_meta };
    else
        target = sInvalid{};
}

void cMaterial::SetDepthTest( const eDepthTest _depth_test )
{
    m_depth_test_ = _depth_test;
}

auto cMaterial::GetDepthTest() const -> eDepthTest
{
    return m_depth_test_;
}

auto cMaterial::GetShaderLink() const -> const Graphics::Utils::cShader_Link&
{
    return m_shader_link_;
}

bool cMaterial::IsReady() const
{
    return m_shader_link_.IsReady();
}

void cMaterial::Update()
{
    for( auto& block : m_block_map_ | std::views::values )
        block.m_buffer_.Upload( true );
}

void cMaterial::create_data()
{
    using namespace sk::Graphics::Utils;

    auto reflection = m_shader_link_.GetReflection();

    // Uniform Blocks
    for( auto& raw_block : reflection->GetBlockVec() )
    {
        auto& block = m_block_map_[ raw_block->name ];
        block = cBlock{ *this, raw_block->pretty_name, raw_block, raw_block->binding };

        const auto data = static_cast< std::byte* >( block.m_buffer_.Data() );
        
        for( auto& raw_uniform : raw_block->uniforms | std::views::values )
        {
            auto& uniform = block.m_uniform_map_[ raw_uniform.name ];
            
            uniform.pretty_name = raw_uniform.pretty_name;
            uniform.array_size  = raw_uniform.size;
            uniform.matrix_size = 1;
            uniform.info        = &raw_uniform;

            switch( raw_uniform.type )
            {
            case sUniform::eType::kFloat:
            case sUniform::eType::kInt:
            case sUniform::eType::kUInt:
                uniform.vector_size = 1; break;
                
            case sUniform::eType::kFloat2x2:
                uniform.matrix_size = 2;
            case sUniform::eType::kFloat2:
            case sUniform::eType::kInt2:
            case sUniform::eType::kUInt2:
                uniform.vector_size = 2; break;
                
            case sUniform::eType::kFloat3x3:
                uniform.matrix_size = 3; break;
            case sUniform::eType::kFloat3:
            case sUniform::eType::kInt3:
            case sUniform::eType::kUInt3:
                uniform.vector_size = 3; break;
                
            case sUniform::eType::kFloat4x4:
                uniform.matrix_size = 4; break;
            case sUniform::eType::kFloat4:
            case sUniform::eType::kInt4:
            case sUniform::eType::kUInt4:
                uniform.vector_size = 4; break;
            }
            
            uniform.byte_size = 4 * uniform.array_size * static_cast< uint32_t >( uniform.vector_size * uniform.matrix_size );
            
            void* data_point = data + raw_uniform.location;
            
#define ACCESSOR_EMPLACE( type ) (uniform.accessor.emplace< type * >( static_cast< type * >( data_point ) ))

            switch( raw_uniform.type )
            {
            case sUniform::eType::kFloat: ACCESSOR_EMPLACE( float );    break;
            case sUniform::eType::kInt:   ACCESSOR_EMPLACE( int32_t );  break;
            case sUniform::eType::kUInt:  ACCESSOR_EMPLACE( uint32_t ); break;
                
            case sUniform::eType::kFloat2: ACCESSOR_EMPLACE( cVector2f ); break;
            case sUniform::eType::kFloat3: ACCESSOR_EMPLACE( cVector3f ); break;
            case sUniform::eType::kFloat4: ACCESSOR_EMPLACE( cVector4f ); break;
                
            case sUniform::eType::kInt2: ACCESSOR_EMPLACE( cVector2i32 ); break;
            case sUniform::eType::kInt3: ACCESSOR_EMPLACE( cVector3i32 ); break;
            case sUniform::eType::kInt4: ACCESSOR_EMPLACE( cVector4i32 ); break;
                
            case sUniform::eType::kUInt2: ACCESSOR_EMPLACE( cVector2u32 ); break;
            case sUniform::eType::kUInt3: ACCESSOR_EMPLACE( cVector3u32 ); break;
            case sUniform::eType::kUInt4: ACCESSOR_EMPLACE( cVector4u32 ); break;
                
            case sUniform::eType::kFloat4x4: ACCESSOR_EMPLACE( cMatrix4x4f ); break;
                
            default: SK_BREAK; break; // Not supported.
            }
        }
    }

    // Samplers/Textures
    auto& samplers = reflection->GetSamplers();
    m_textures_.resize( samplers.size() );
    
    size_t i = 0;
    for( auto& sampler : samplers | std::views::values )
    {
        auto& texture = m_textures_[ i++ ] = { .index = i, .sampler = &sampler, .texture = sInvalid{} };
        m_sampler_map_[ sampler.name ] = &texture;
    }
}
