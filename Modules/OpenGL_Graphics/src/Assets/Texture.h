/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>

#include <glbinding/gl/types.h>

#include <stb_image.h>

namespace sk::Assets
{
    // TODO: Create unsafe texture.
    // TODO: Look at https://github.com/nothings/stb/blob/master/stb_image.h for importing textures.

    class cUnsafe_Texture
    {
    public:
        gl::GLuint m_buffer_;
    };

    SK_ASSET_CLASS( Texture )
    {
        SK_CLASS_BODY( Texture )
    sk_public:
        enum class eSourceType : uint8_t
        {
            kInvalid,
            kPNG,
            kJPG
        };

        cTexture( const std::string& _name, const void* _buffer, size_t _size, eSourceType _type );

        void Save() override {}

    sk_private:
        cUnsafe_Texture m_buffer_;
    };
    
} // sk::Assets

REGISTER_CLASS( sk::Assets::Texture )