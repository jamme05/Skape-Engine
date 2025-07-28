/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Shader.h"

#include <glbinding/gl/functions.h>

namespace sk::Assets
{
    cShader::cShader( const std::string& _name, const eType _type, const void* _buffer, const size_t _size )
    : cAsset( _name )
    {
        m_type_ = gl::GL_INVALID_VALUE;
        switch( _type )
        {
        case eType::kVertex:   m_type_ = gl::GL_VERTEX_SHADER;   m_vertex_shader_   = get_shared_this(); break;
        case eType::kFragment: m_type_ = gl::GL_FRAGMENT_SHADER; m_fragment_shader_ = get_shared_this(); break;
        case eType::kGeometry: m_type_ = gl::GL_GEOMETRY_SHADER; break;
        }

        SK_ERR_IF( m_type_ == gl::GL_INVALID_VALUE,
            "ERROR: Invalid shader type." )

        SK_ERR_IF( m_type_ == gl::GL_GEOMETRY_SHADER,
            "ERROR: Geometry shaders aren't supported." )
        
        m_shader_ = gl::glCreateShader( m_type_ );
        const auto str = static_cast< const gl::GLchar* >( _buffer );
        const auto size = static_cast< const gl::GLsizei >( _size );
        gl::glShaderSource( m_shader_, 1, &str, &size );
        gl::glCompileShader( m_shader_ );
    } // cShader

    cShader::~cShader()
    {
        gl::glDeleteShader ( m_shader_ );
        gl::glDeleteProgram( m_program_ );
    } // ~cShader

    cShader& cShader::LinkShader( const cShader& _other_shader )
    {
        SK_ERR_IF( m_type_ == _other_shader.m_type_,
            "ERROR: Trying to link same types of shaders." )

        switch( _other_shader.m_type_ )
        {
        case gl::GL_VERTEX_SHADER:   m_vertex_shader_   = _other_shader.m_vertex_shader_;   break;
        case gl::GL_FRAGMENT_SHADER: m_fragment_shader_ = _other_shader.m_fragment_shader_; break;
        default: SK_FATAL( "ERROR: Invalid shader type." )
        }

        link_shaders();

        return *this;
    } // LinkShader

    cShader& cShader::LinkShader( [[ maybe_unused ]] eType _type, const cShader& _other_shader )
    {
        // Reserved in case other behaviour would be liked.
        return LinkShader( _other_shader );
    } // LinkShader

    void cShader::link_shaders()
    {
        m_program_ = gl::glCreateProgram();
        gl::glAttachShader( m_program_, m_vertex_shader_->m_shader_ );
        gl::glAttachShader( m_program_, m_fragment_shader_->m_shader_ );
        gl::glLinkProgram( m_program_ );
    }
} // sk::Assets