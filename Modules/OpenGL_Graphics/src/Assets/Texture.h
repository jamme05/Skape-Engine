/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>

#include <glbinding/gl/types.h>

#include "Math/Vector2.h"

namespace sk::Assets
{
    // TODO: Create unsafe texture.

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

        enum eChannel : uint8_t
        {
            // Invalid
            kNone  = 0,

            // Channels
            kRed    = 0x01,
            kR      = kRed,
            kGreen  = 0x02,
            kG      = kGreen,
            kBlue   = 0x04,
            kB      = kBlue,
            // Alpha in the back, example: RGBA
            kAlpha  = 0x08,
            // Alpha in the back, example: RGBA
            kA      = kAlpha,
            // Alpha in the front, example ARGB
            kFAlpha = 0x10,
            // Alpha in the front, example ARGB
            kFA     = kFAlpha,


            // Settings
            // In case the order is reversed.
            kReverse = 0x20,

            // Types
            kGray  = kRed,
            kGrey  = kRed,

            kGA    = kRed | kAlpha,
            kAG    = kRed | kFAlpha,

            kRG    = kRed | kGreen,
            kRGB   = kRed | kGreen | kBlue,
            kRGBA  = kRed | kGreen | kBlue | kAlpha,
            kARGB  = kRed | kGreen | kBlue | kFAlpha,

            kGR    = kRG  | kReverse,
            kBGR   = kRGB | kReverse,
            kABGR  = kRGB | kReverse,
        };

        // TODO: Texture settings/Sampler
        cTexture( const std::string& _name, const void* _buffer, size_t _size );

    sk_private:
        uint8_t         m_channels_;
        cVector2u32       m_size_;
        cUnsafe_Texture m_buffer_;
    };
    
} // sk::Assets

DECLARE_CLASS( sk::Assets::Texture )
