/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Math/Vector2.h>
#include <glbinding/gl/types.h>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Misc/Smart_Ptrs.h"

namespace sk::Assets
{
    class cTexture;
} // sk::Assets

namespace sk::Graphics::Rendering
{
    class cDepth_Target
    {
        friend class cFrame_Buffer;
    public:
        // TODO: Move to the engine.
        enum class eFormat : uint8_t
        {
            kD16F,
            kD24FS8,
            kD32F,
        };
        
        explicit cDepth_Target( const cVector2u32& _resolution, eFormat _format, float _depth_clear = 0.0f, uint8_t _stencil_clear = 0 );

        void GetResolution() const -> cVector2u32;
        void Resize( const cVector2u32& _new_resolution );

        auto GetClearValue() const { return m_depth_clear_; }
        void SetClearValue( float _value );

        cShared_ptr< Assets::cTexture > CreateTexture() const;
    private:
        void create ();
        void destroy();

        cVector2u32  m_resolution_;
        cVector2f    m_rel_scale_ = kOne;
        gl::GLint    m_stencil_clear_;
        eFormat      m_format_;
        gl::GLenum   m_buffer_type_;
        float        m_depth_clear_;
        gl::GLuint   m_buffer_;
    };
} // sk::Graphics::Rendering

