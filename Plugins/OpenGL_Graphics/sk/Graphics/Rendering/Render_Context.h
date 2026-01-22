/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Containers/Vector.h>
#include <sk/Math/Vector2.h>

#include <glbinding/gl/types.h>

namespace sk::Graphics
{
    struct sViewport;
    struct sScissor;
} // sk::Graphics

namespace sk::Graphics
{
    class cRenderer;
} // sk::Graphics

namespace sk::Graphics::Rendering
{
    class cFrame_Buffer;
} // sk::Graphics::Rendering

namespace sk::Graphics::Rendering
{
    class cRender_Context
    {
    public:
        explicit cRender_Context( size_t _frame_buffers = 2, size_t _render_targets = 1 );
        virtual ~cRender_Context();

        void Resize( const cVector2u32& _new_resolution ) const;

        void Begin( const sViewport& _viewport, const sScissor& _scissor ) const;
        void End  ( bool _swap_buffers = true );
        void Clear( uint8_t _clear = 0xff ) const;

        [[nodiscard]] auto& GetBack () const { return *m_back_;  }
        [[nodiscard]] auto& GetFront() const { return *m_front_; }

        virtual void Swap();
        
        auto begin()       { return m_frame_buffers_.begin(); }
        auto begin() const { return m_frame_buffers_.begin(); }
        auto end  ()       { return m_frame_buffers_.end();   }
        auto end  () const { return m_frame_buffers_.end();   }
        
    protected:
        cRender_Context( size_t _frame_buffers, size_t _render_targets, bool _is_window );
        
    private:
        friend class sk::Graphics::cRenderer;

        using frame_buffer_vec_t = vector< cFrame_Buffer* >;
        frame_buffer_vec_t m_frame_buffers_;

        cFrame_Buffer* m_front_;
        cFrame_Buffer* m_back_;

        size_t   m_front_index_ = 0;
        uint64_t m_frame_       = 0;
    };
} // sk::Graphics::Rendering
