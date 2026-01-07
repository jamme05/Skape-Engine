/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Render_Context.h"

#include <glbinding/gl/functions.h>
#include <Graphics/Rendering/Frame_Buffer.h>

namespace sk::Graphics::Rendering
{
    cRender_Context::cRender_Context( const size_t _frame_buffers, const size_t _render_targets )
    : cRender_Context( _frame_buffers, _render_targets, false )
    {} // cRender_Context
    
    cRender_Context::cRender_Context( const size_t _frame_buffers, const size_t _render_targets, const bool _is_window )
    : m_frame_buffers_( _frame_buffers )
    {
        SK_ERR_IF( _frame_buffers == 0,
                   "ERROR: Can't create a render context without frame buffers." )

        // Initialize frame buffers.
        for( size_t i = 0; i < _frame_buffers; ++i )
        {
            const auto& frame_buffer = m_frame_buffers_[ i ] = SK_SINGLE( cFrame_Buffer, _render_targets );
            frame_buffer->create( _is_window );
        }
        m_front_ = m_frame_buffers_.front();
        m_back_ = m_frame_buffers_.back();
    }

    cRender_Context::~cRender_Context()
    {
        for( const auto& frame_buffer : m_frame_buffers_ )
            SK_DELETE( frame_buffer );

        m_frame_buffers_.clear();
    } // ~cRender_Context

    void cRender_Context::Resize( const cVector2u32& _new_resolution ) const
    {
        for( const auto& frame_buffer : m_frame_buffers_ )
            frame_buffer->Resize( _new_resolution );
    } // Resize

    void cRender_Context::Begin( const sViewport& _viewport, const sScissor& _scissor ) const
    {
        // TODO: Possibly deprecate?
        m_back_->Begin( _viewport, _scissor );
    } // Begin

    void cRender_Context::End( const bool _swap_buffers )
    {
        m_back_->End();

        if( _swap_buffers )
            Swap();
    } // End

    void cRender_Context::Clear( const uint8_t _clear ) const
    {
        for( const auto& frame_buffer : m_frame_buffers_ )
            frame_buffer->Clear( _clear );
    } // Clear

    void cRender_Context::Swap()
    {
        m_back_->EndFrame();

        m_back_ = m_front_;

        m_front_index_ = ++m_frame_ % m_frame_buffers_.size();

        m_front_ = m_frame_buffers_[ m_front_index_ ];

        m_front_->BeginFrame();
    } // Swap
} // sk::Graphics::Rendering
