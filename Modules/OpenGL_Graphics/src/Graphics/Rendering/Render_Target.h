/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

#include <Misc/Smart_Ptrs.h>

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace sk::Assets
{
    class cTexture;
} // sk::Assets

namespace sk::Graphics::Rendering
{
    // TODO: Rename to render target 2d or allow it multiple dimensions.
    // For scalability, adding more classes should be better.
    class cRender_Target
    {
        friend class cFrame_Buffer;
    public:
        enum class eType : uint8_t
        {
            // Map these to the GLEnum types or nah?
            // Depends on if I'm gonna move it to another place.
            kByte,
            kInt,
            kUint,
            kHalfFloat,
            kFloat,
        };

        // TODO: Add RG formats.
        enum class eFormat : uint8_t
        {
            // R
            kR16F,
            kR32F,
            // RGB
            kRGB8,
            kRGB16F,
            kRGB32F,
            // RGBA
            kRGBA8,
            kRGBA16F,
            kRGBA32F,
        };

        cRender_Target( const cVector2u32& _resolution, eFormat _format, const cColor& _clear_color = Color::kBlack );

        void Resize( const cVector2u32& _resolution );

        auto& GetClearColor() const { return m_clear_color_; }
        void  SetClearColor( const cColor& _clear_color );

        // Internal use only.
        auto get_texture() const { return m_texture_; }

    private:
        // YOU NEED TO BIND A FRAME BUFFER BEFORE CALLING THIS!!!!
        void clear( gl::GLint _index );
        void create();
        void destroy() const;

        cColor      m_clear_color_;
        cVector2u32 m_resolution_;
        // The scale relative to the first render target in a owning frame buffer. Is 1,1 in the first.
        cVector2f   m_rel_scale_ = kOne;
        eFormat     m_format_;
        eType       m_type_;
        gl::GLuint  m_texture_;
    };
} // sk::Graphics::Rendering

