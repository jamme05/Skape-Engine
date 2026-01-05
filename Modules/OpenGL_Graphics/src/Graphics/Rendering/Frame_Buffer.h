/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Containers/Vector.h>
#include <glbinding/gl/types.h>

#include <Misc/Smart_Ptrs.h>

#include "Graphics/Buffer/Dynamic_Buffer.h"
#include "Math/Vector2.h"

namespace sk::Graphics::Rendering
{
    class cRender_Target;
}

namespace sk::Graphics
{
    namespace OpenGL
    {
        class cUnsafe_Buffer;
    }

    struct sScissor;
    struct sViewport;
} // sk::Graphics

namespace sk::Graphics::Rendering
{
    class cDepth_Target;
} // sk::Graphics::Rendering

namespace sk::Graphics::Rendering
{
    // TODO: Move to a more accessible place.
    enum eClear : uint8_t
    {
        kNone    = 0,
        kTargets = 0x01,
        kDepth   = 0x02,
        kStencil = 0x04,
        kDepthStencil = kDepth | kStencil,

        kAll = 0xff,
    };

    class cFrame_Buffer
    {
    public:

        // TODO: Add name to frame buffer?
        explicit cFrame_Buffer( size_t _render_targets = 1 );
        ~cFrame_Buffer();

        void BeginFrame();
        void EndFrame();

        void Begin( const sViewport& _viewport, const sScissor& _scissor );
        void End();

        void Clear( uint8_t _clear );

        // The target NEEDS to be able to survive.
        void Bind( size_t _index, const cShared_ptr< cRender_Target >& _target, bool _force = false );
        void Bind( const cShared_ptr< cDepth_Target >& _depth_target, bool _force = false );
        
        void BindVertexBuffer( size_t _binding, const cDynamic_Buffer& _buffer );
        void UnbindVertexBuffers();

        void UnbindRenderTargetAt( size_t _index );
        void UnbindDepthTarget();

        // When resizing the buffer.
        void Resize( const cVector2u32& _new_resolution );

    private:
        friend class cRender_Context;

        void create ();
        void destroy() const;

        gl::GLuint m_frame_buffer_;
        gl::GLenum m_depth_type_;
        vector< cShared_ptr< cRender_Target > > m_render_targets_;
        cShared_ptr< cDepth_Target >            m_depth_target_;
    };
} // sk::Graphics::Rendering

// TODO: Move Clear out of frame buffer.
namespace sk::Clear
{
    using enum Graphics::Rendering::eClear;
} // sk::Clear
