/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Shader.h"

#include <Math/Types.h>

#include <glbinding/gl/functions.h>

#include "Graphics/Buffer/Dynamic_Buffer.h"
#include "Math/Matrix3x3.h"

namespace sk::Assets
{
    // Shader reflection:
    cShader::cShader_Reflection::cShader_Reflection( const gl::GLuint _program )
    : m_program_( _program )
    {
        init();
        
        get_attributes();
        
        get_blocks();
        get_uniforms();
        
        cleanup();
    }

    void cShader::cShader_Reflection::init()
    {
        gl::GLint out;
        gl::glGetProgramiv( m_program_, gl::GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &out );
        m_max_attribute_name_size_ = static_cast< uint32_t >( out );

        gl::glGetProgramiv( m_program_, gl::GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &out );
        m_max_block_name_size_ = static_cast< uint32_t >( out );

        gl::glGetProgramiv( m_program_, gl::GL_ACTIVE_UNIFORM_MAX_LENGTH, &out );
        m_max_uniform_name_size_ = static_cast< uint32_t >( out );
        m_uniform_name_buffer_ = static_cast< gl::GLchar* >( SK_ALLOC( m_max_uniform_name_size_ ) );
    }

    void cShader::cShader_Reflection::get_attributes()
    {
        gl::GLint nr_of_attributes;
        gl::glGetProgramiv( m_program_, gl::GL_ACTIVE_ATTRIBUTES, &nr_of_attributes );

        const auto a_name_buffer = static_cast< gl::GLchar* >( SK_ALLOC( m_max_attribute_name_size_ ) );
        
        for( int_fast32_t i = 0; i < nr_of_attributes; i++ )
        {
            auto& attribute = m_attributes_.emplace_back();
            
            gl::GLsizei name_length;
            gl::GLsizei size;
            gl::GLenum  type;
            
            gl::glGetActiveAttrib( m_program_, i, static_cast< gl::GLint >( m_max_attribute_name_size_ ),
                &name_length, &size, &type, a_name_buffer );
            
            attribute.name   = std::string_view( a_name_buffer, name_length - 1 );
            attribute.index  = i;
            attribute.stride = size;

            type_info_t sk_type;
            switch( type )
            {
            case gl::GL_FLOAT:      sk_type = &kTypeInfo< float >;     break;
            case gl::GL_FLOAT_VEC2: sk_type = &kTypeInfo< cVector2f >; break;
            case gl::GL_FLOAT_VEC3: sk_type = &kTypeInfo< cVector3f >; break;
            case gl::GL_FLOAT_VEC4: sk_type = &kTypeInfo< cVector4f >; break;
                
            case gl::GL_INT:       sk_type = &kTypeInfo< int32_t >;     break;
            case gl::GL_INT_VEC2:  sk_type = &kTypeInfo< cVector2i32 >; break;
            case gl::GL_INT_VEC3:  sk_type = &kTypeInfo< cVector3i32 >; break;
            case gl::GL_INT_VEC4:  sk_type = &kTypeInfo< cVector4i32 >; break;
                
            case gl::GL_UNSIGNED_INT:      sk_type = &kTypeInfo< uint32_t >;    break;
            case gl::GL_UNSIGNED_INT_VEC2: sk_type = &kTypeInfo< cVector2u32 >; break;
            case gl::GL_UNSIGNED_INT_VEC3: sk_type = &kTypeInfo< cVector3u32 >; break;
            case gl::GL_UNSIGNED_INT_VEC4: sk_type = &kTypeInfo< cVector4u32 >; break;
                
            case gl::GL_FLOAT_MAT2: sk_type = &kTypeInfo< Math::cMatrix< 2, 2, float > >; break;
            case gl::GL_FLOAT_MAT3: sk_type = &kTypeInfo< Math::cMatrix3x3f >;            break;
            case gl::GL_FLOAT_MAT4: sk_type = &kTypeInfo< cMatrix4x4f >;                  break;
                
            default: sk_type = nullptr; break;
            }
            
            SK_ERR_IF( sk_type == nullptr,
                "Error: Invalid attribute type." )
            
            attribute.type = sk_type;
        }
        
        SK_FREE( a_name_buffer );
    }

    void cShader::cShader_Reflection::get_uniforms()
    {
        gl::GLint nr_of_uniforms;
        gl::glGetProgramiv( m_program_, gl::GL_ACTIVE_UNIFORM_BLOCKS, &nr_of_uniforms );
        
        for( int_fast32_t i = 0; i < nr_of_uniforms; i++ )
        {
            if( m_locked_uniforms_.contains( i ) )
                continue;
            
            auto uniform = get_uniform( i );
            m_global_uniforms_[ uniform.name.hash() ] = std::move( uniform );
        }
    }

    void cShader::cShader_Reflection::get_blocks()
    {
        gl::GLint nr_of_blocks;
        gl::glGetProgramiv( m_program_, gl::GL_ACTIVE_UNIFORM_BLOCKS, &nr_of_blocks );
        
        if( nr_of_blocks == 0 )
            return;
        
        m_blocks_.reserve( nr_of_blocks );

        const auto b_name_buffer = static_cast< char* >( SK_ALLOC( m_max_block_name_size_ ) );
        
        for( int_fast32_t i = 0; i < nr_of_blocks; i++ )
        {
            gl::GLsizei name_length;
            gl::glGetActiveUniformBlockName( m_program_, i, static_cast< gl::GLsizei >( m_max_block_name_size_ ),
                &name_length, b_name_buffer );
            
            auto  name   = std::string_view( b_name_buffer, name_length - 1 );
            auto& block = m_blocks_[ name ];
            block.name = name;
            
            gl::GLint nr_of_uniforms;
            gl::glGetActiveUniformBlockiv( m_program_, i, gl::GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &nr_of_uniforms );

            auto indices = std::vector< gl::GLint >{};
            indices.resize( static_cast< size_t >( nr_of_uniforms ) ); 
            
            gl::glGetActiveUniformBlockiv( m_program_, i, gl::GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices.data() );
            
            block.uniforms.reserve( static_cast< size_t >( nr_of_uniforms ) );
            uint32_t offset = 0;
            for( int_fast32_t u = 0; u < nr_of_uniforms; u++ )
            {
                auto uniform = get_uniform( indices[ u ] );
                
                uniform.location = offset;
                offset += uniform.size;
                
                m_locked_uniforms_.insert( uniform.location );
                block.uniforms[ uniform.name.hash() ] = std::move( uniform );
            }
        }
        
        SK_FREE( b_name_buffer );
    }

    cShader::cShader_Reflection::sUniform cShader::cShader_Reflection::get_uniform( const gl::GLuint _index ) const
    {
        gl::GLsizei name_length;
        gl::GLsizei size;
        gl::GLenum  type;
        
        gl::glGetActiveUniform( m_program_, _index, static_cast< gl::GLsizei >( m_max_block_name_size_ ), 
            &name_length, &size, &type, m_uniform_name_buffer_ );
        
        sUniform uniform;
        uniform.name     = std::string_view( m_uniform_name_buffer_, name_length - 1 );
        uniform.type     = type;
        uniform.location = _index;
        uniform.size     = size;
        
        return uniform;
    }

    void cShader::cShader_Reflection::cleanup()
    {
        SK_FREE( m_uniform_name_buffer_ );
        m_uniform_name_buffer_ = nullptr;
    }

    // Shader
    cShader::cShader( const eType _type, const void* _buffer, const size_t _size )
    {
        m_type_ = gl::GL_INVALID_VALUE;
        switch( _type )
        {
        case eType::kVertex:   m_type_ = gl::GL_VERTEX_SHADER;   break;
        case eType::kFragment: m_type_ = gl::GL_FRAGMENT_SHADER; break;
        case eType::kGeometry: m_type_ = gl::GL_GEOMETRY_SHADER; break;
        case eType::kCompute:  m_type_ = gl::GL_COMPUTE_SHADER;  break;
        }
        
        LinkShader( *get_weak().cast< cShader >() );

        SK_ERR_IF( m_type_ == gl::GL_INVALID_VALUE,
            "ERROR: Invalid shader type." )

        SK_ERR_IF( m_type_ == gl::GL_GEOMETRY_SHADER,
            "ERROR: Geometry shaders aren't supported yet." )

        SK_ERR_IF( m_type_ == gl::GL_COMPUTE_SHADER,
            "ERROR: Compute shaders aren't supported yet." )
        
        m_program_ = gl::glCreateProgram();
        
        m_shader_ = gl::glCreateShader( m_type_ );
        const auto str = static_cast< const gl::GLchar* >( _buffer );
        const auto size = static_cast< const gl::GLsizei >( _size );
        gl::glShaderSource( m_shader_, 1, &str, &size );
        gl::glCompileShader( m_shader_ );
        
        m_reflection_ = sk::make_shared< cShader_Reflection >( m_program_ );
    } // cShader

    cShader::~cShader()
    {
        UnlinkShader( eType::kVertex );
        UnlinkShader( eType::kFragment );
        
        gl::glDeleteShader ( m_shader_ );
        gl::glDeleteProgram( m_program_ );
    } // ~cShader

    cShader& cShader::LinkShader( const cShader& _other_shader )
    {
        SK_ERR_IF( m_type_ == _other_shader.m_type_,
            "ERROR: Trying to link same types of shaders." )

        switch( _other_shader.m_type_ )
        {
        case gl::GL_VERTEX_SHADER:
        {
            if( m_vertex_shader_ != nullptr )
                UnlinkShader( eType::kVertex );
            m_vertex_shader_ = _other_shader.m_vertex_shader_;
        }
        break;
        case gl::GL_FRAGMENT_SHADER:
        {
            if( m_vertex_shader_ != nullptr )
                UnlinkShader( eType::kFragment );
            m_fragment_shader_ = _other_shader.m_fragment_shader_;
        }
        break;
        default: SK_FATAL( "ERROR: Invalid shader type." )
        }

        link_shaders();

        return *this;
    } // LinkShader

    cShader& cShader::LinkShader( [[ maybe_unused ]] eType _type, const cShader& _other_shader )
    {
        // Reserved in case another behavior would be liked.
        return LinkShader( _other_shader );
    } // LinkShader
    
    void cShader::UnlinkShader( const eType _type )
    {
        cWeak_Ptr< cShader > shader;
        switch( _type )
        {
        case eType::kVertex:   shader = m_vertex_shader_;   m_vertex_shader_   = nullptr; break;
        case eType::kFragment: shader = m_fragment_shader_; m_fragment_shader_ = nullptr; break;
        default: shader = nullptr; break;
        }
        
        if( shader == nullptr )
            return;
        
        gl::glDetachShader( m_program_, shader->m_shader_ );
    }

    void cShader::link_shaders() const
    {
        gl::glLinkProgram( m_program_ );
    }
} // sk::Assets