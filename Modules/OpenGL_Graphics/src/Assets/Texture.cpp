/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Texture.h"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

// TODO: Move stb to OpenGL library.
// TODO: Look at https://github.com/nothings/stb/blob/master/stb_image.h for importing textures.
#include <stb_image.h>

REGISTER_CLASS( sk::Assets::Texture )

namespace sk::Assets
{
    cTexture::cTexture( const std::string& _name, const void* _buffer, const size_t _size )
    : cAsset( _name )
    {
        // TODO: Add load/destroy functions to assets.

        // TODO: Maybe move the texture creation to it's own file? As a texture loader ish, like tengine does it.
        // https://learnopengl.com/Getting-started/Textures
        int width, height, channels;
        const auto data = stbi_load_from_memory( static_cast< const stbi_uc* >( _buffer ), static_cast< int >( _size ), &width, &height, &channels, 0 );

        SK_ERR_IF( data == nullptr,
            TEXT( "ERROR: Failed to load texture with name", _name ) )

        m_size_  = { width, height };
        m_channels_ = static_cast< uint8_t >( channels );

        __builtin_assume( m_channels_ <= 4 && m_channels_ > 0 );

        gl::GLenum format;
        switch( m_channels_ )
        {
        case kR:    format = gl::GLenum::GL_R;    break;
        case kRG:   format = gl::GLenum::GL_RG;   break;
        case kRGB:  format = gl::GLenum::GL_RGB;  break;
        case kRGBA: format = gl::GLenum::GL_RGBA; break;
        default:    format = gl::GLenum::GL_INVALID_VALUE; break;
        }

        SK_ERR_IF( format == gl::GLenum::GL_INVALID_VALUE,
            TEXT( "ERROR: Texture with name {} does not have a valid ", _name ) )

        gl::glGenTextures( 1, &m_buffer_.m_buffer_ );
        gl::glBindTexture( gl::GL_TEXTURE_2D, m_buffer_.m_buffer_ );
        gl::glTexImage2D ( gl::GL_TEXTURE_2D, 0, static_cast< gl::GLint >( format ), width, height, 0, format, gl::GL_UNSIGNED_BYTE, data );
        gl::glGenerateMipmap( gl::GL_TEXTURE_2D );
        gl::glBindTexture( gl::GL_TEXTURE_2D, 0 );

        stbi_image_free( data );
    }
} // sk::Assets
