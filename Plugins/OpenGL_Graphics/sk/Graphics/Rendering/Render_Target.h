/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Math/Vector2.h>
#include <sk/Math/Vector3.h>
#include <sk/Math/Vector4.h>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

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

        [[ nodiscard ]]
        auto GetResolution() const -> cVector2u32;
        void Resize( cVector2u32 _resolution, bool _use_relative = false );

        [[ nodiscard ]]
        auto& GetClearColor() const { return m_clear_color_; }
        void  SetClearColor( const cColor& _clear_color );

        // Internal use only.
        [[ nodiscard ]]
        auto get_texture_object() const { return m_texture_; }
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

