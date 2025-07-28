/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Depth_Target.h"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

namespace sk::Graphics::Rendering
{
    cDepth_Target::cDepth_Target( const cVector2u32& _resolution, const eFormat _format, const float _depth_clear, const uint8_t _stencil_clear )
    : m_resolution_( _resolution )
    , m_stencil_clear_( _stencil_clear )
    , m_format_( _format )
    , m_depth_clear_( _depth_clear )
    {
        create();
    } // cDepth_Target

    void cDepth_Target::Resize( const cVector2u32& _new_resolution )
    {
        destroy();

        m_resolution_ = _new_resolution;
        
        create();
    } // Resize

    void cDepth_Target::SetClearValue( const float _value )
    {
        m_depth_clear_ = _value;
    } // SetClearValue

    cShared_ptr< Assets::cTexture > cDepth_Target::CreateTexture() const
    {
        // TODO: Depth buffer to texture conversion.
        // https://stackoverflow.com/a/6341650
        return {};
    }

    void cDepth_Target::create()
    {
        auto format = gl::GLenum::GL_INVALID_VALUE;
        switch( m_format_ )
        {
        case eFormat::kD16F:   format = gl::GL_DEPTH_COMPONENT16; m_buffer_type_ = gl::GL_DEPTH_ATTACHMENT;         break;
        case eFormat::kD24FS8: format = gl::GL_DEPTH24_STENCIL8;  m_buffer_type_ = gl::GL_DEPTH_STENCIL_ATTACHMENT; break;
        case eFormat::kD32F:   format = gl::GL_DEPTH_COMPONENT32; m_buffer_type_ = gl::GL_DEPTH_ATTACHMENT;         break;
        }

        SK_ERR_IF( format == gl::GLenum::GL_INVALID_VALUE,
            "ERROR: Invalid Depth buffer format." )

        const Math::cVector2< gl::GLsizei > res = m_resolution_;
        gl::glGenRenderbuffers( 1, &m_buffer_ );
        gl::glBindRenderbuffer( gl::GLenum::GL_RENDERBUFFER, m_buffer_ );
        gl::glNamedRenderbufferStorage( m_buffer_, format, res.x, res.y );
        gl::glBindRenderbuffer( gl::GLenum::GL_RENDERBUFFER, 0 );
    } // create

    void cDepth_Target::destroy()
    {
        gl::glDeleteRenderbuffers( 1, &m_buffer_ );
    } // destroy
} // sk::Graphics::Rendering