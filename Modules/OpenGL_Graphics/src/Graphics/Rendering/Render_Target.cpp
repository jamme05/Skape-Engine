/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Render_Target.h"

#include <glbinding/gl/functions.h>

namespace sk::Graphics::Rendering
{
    cRender_Target::cRender_Target( const cVector2u32& _resolution, const eFormat _format, const cColor& _clear_color )
    : m_clear_color_( _clear_color )
    , m_resolution_ ( _resolution )
    , m_format_     ( _format )
    {
        create();
    }

    void cRender_Target::Resize( const cVector2u32& _resolution )
    {
        destroy();

        m_resolution_ = _resolution;

        create();
    }

    void cRender_Target::SetClearColor( const cColor& _clear_color )
    {
        m_clear_color_ = _clear_color;
    } // SetClearColor

    void cRender_Target::clear( const gl::GLint _index )
    {
        switch( m_type_ )
        {
        case eType::kInt:
        {
            const Math::cVector4< gl::GLint > clear_color = m_clear_color_;
            gl::glClearBufferiv( gl::GL_COLOR, _index, &clear_color.x );
        }
        break;
        case eType::kByte:
        case eType::kUint:
        {
            const Math::cVector4< gl::GLuint > clear_color = m_clear_color_;
            gl::glClearBufferuiv( gl::GL_COLOR, _index, &clear_color.x );
        }
        break;
        case eType::kHalfFloat:
        case eType::kFloat:
            gl::glClearBufferfv( gl::GL_COLOR, _index, &m_clear_color_.x );
        break;
        }
    }

    void cRender_Target::create()
    {
        // TODO: Cache the result for easier recreation?
        gl::GLenum format = gl::GL_INVALID_VALUE;
        gl::GLenum type   = gl::GL_INVALID_VALUE;
        switch( m_format_ )
        {
        // R
        case eFormat::kR16F:    format = gl::GL_R;    type = gl::GL_HALF_FLOAT;    m_type_ = eType::kHalfFloat; break;
        case eFormat::kR32F:    format = gl::GL_R;    type = gl::GL_FLOAT;         m_type_ = eType::kFloat;     break;
        // RGB
        case eFormat::kRGB8:    format = gl::GL_RGB;  type = gl::GL_UNSIGNED_BYTE; m_type_ = eType::kByte;      break;
        case eFormat::kRGB16F:  format = gl::GL_RGB;  type = gl::GL_HALF_FLOAT;    m_type_ = eType::kHalfFloat; break;
        case eFormat::kRGB32F:  format = gl::GL_RGB;  type = gl::GL_FLOAT;         m_type_ = eType::kFloat;     break;
        // RGBA
        case eFormat::kRGBA8:   format = gl::GL_RGBA; type = gl::GL_UNSIGNED_BYTE; m_type_ = eType::kByte;      break;
        case eFormat::kRGBA16F: format = gl::GL_RGBA; type = gl::GL_HALF_FLOAT;    m_type_ = eType::kHalfFloat; break;
        case eFormat::kRGBA32F: format = gl::GL_RGBA; type = gl::GL_FLOAT;         m_type_ = eType::kFloat;     break;
        }

        SK_ERR_IF( format == gl::GL_INVALID_VALUE, "ERROR: Invalid format." )

        const Math::cVector2< gl::GLsizei > res = m_resolution_;

        gl::glGenTextures( 1, &m_texture_ );
        gl::glBindTexture( gl::GL_TEXTURE_2D, m_texture_ );
        gl::glTexImage2D ( gl::GL_TEXTURE_2D, 0, format, res.x, res.y, 0, format, type, nullptr );
        gl::glBindTexture( gl::GL_TEXTURE_2D, 0 );
    } // create

    void cRender_Target::destroy() const
    {
        gl::glDeleteTextures( 1, &m_texture_ );
    } // destroy
} // sk::Graphics::Rendering