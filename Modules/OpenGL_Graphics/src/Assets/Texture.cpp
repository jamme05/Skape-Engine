/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Texture.h"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

namespace sk::Assets
{
    cTexture::cTexture( const std::string& _name, const void* _buffer, size_t _size, eSourceType _type )
    : cAsset( _name )
    {
        // https://learnopengl.com/Getting-started/Textures
        gl::glGenTextures( 1, &m_buffer_.m_buffer_ );
        gl::glBindTexture( gl::GLenum::GL_TEXTURE_2D, m_buffer_.m_buffer_ );
    }

}
