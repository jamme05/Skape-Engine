/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Frame_Buffer.h"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

#include "Depth_Target.h"
#include "Render_Target.h"
#include "Scissor.h"
#include "Viewport.h"
#include "Graphics/Buffer/Unsafe_Buffer.h"

namespace sk::Graphics::Rendering
{
    cFrame_Buffer::cFrame_Buffer( const size_t _render_targets )
    : m_render_targets_( _render_targets )
    {
        SK_ERR_IF( _render_targets == 0, "ERROR: Can't create a frame buffer with 0 render targets." )
        SK_ERR_IF( _render_targets > 31, "ERROR: OpenGL doesn't support more than 31 render targets." )
        create();
    } // cFrame_Buffer

    cFrame_Buffer::~cFrame_Buffer()
    {
        m_render_targets_.clear();
        destroy();
    } // ~cFrame_Buffer

    void cFrame_Buffer::BeginFrame()
    {
    } // BeginFrame

    void cFrame_Buffer::EndFrame()
    {
    } // EndFrame

    void cFrame_Buffer::Begin( const sViewport& _viewport, const sScissor& _scissor )
    {
        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, m_frame_buffer_ );
        gl::glViewport( _viewport.x, _viewport.y, static_cast< gl::GLsizei >( _viewport.width ), static_cast< gl::GLsizei >( _viewport.height ) );
        gl::glScissor ( _scissor.x,   _scissor.y, static_cast< gl::GLsizei >(  _scissor.width ), static_cast< gl::GLsizei >(  _scissor.height ) );
    } // Begin

    void cFrame_Buffer::End()
    {
        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );
    } // End

    void cFrame_Buffer::create()
    {
        gl::glGenFramebuffers( 1, &m_frame_buffer_ );
        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, m_frame_buffer_ );
        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );
    } // create

    void cFrame_Buffer::destroy() const
    {
        gl::glDeleteFramebuffers( 1, &m_frame_buffer_ );
    } // destroy

    void cFrame_Buffer::Clear( const uint8_t _clear )
    {
        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, m_frame_buffer_ );
        // Clear render targets
        if( _clear & kTargets )
        {
            for( size_t i = 0; i < m_render_targets_.size(); i++ )
                m_render_targets_[ i ]->clear( static_cast< gl::GLint >( i ) );
        }

        // Clear depth and stencil
        if( ( _clear & kDepthStencil ) == kDepthStencil )
            gl::glClearBufferfi( gl::GL_DEPTH_STENCIL, 0, m_depth_target_->m_depth_clear_, m_depth_target_->m_stencil_clear_ );
        else if( _clear & kDepth )
            gl::glClearBufferfv( gl::GL_DEPTH, 0, &m_depth_target_->m_depth_clear_ );
        else if( _clear & kStencil )
            gl::glClearBufferiv( gl::GL_STENCIL, 0, &m_depth_target_->m_stencil_clear_ );

        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );
    } // Clear

    void cFrame_Buffer::Bind( const size_t _index, const cShared_ptr< cRender_Target >& _target, const bool _force )
    {
        SK_ERR_IF( _index > m_render_targets_.size(),
            TEXT( "ERROR: Frame buffer was only created for {} render targets.", m_render_targets_.size() ) )

        if( m_render_targets_[ _index ] == _target && !_force )
            return;

        if( m_render_targets_[ _index ] )
            UnbindRenderTargetAt( _index );

        // The GL_COLOR_ATTACKMENTX are after one another. Making this safe.
        const auto attachment = static_cast< gl::GLenum >( static_cast< size_t >( gl::GL_COLOR_ATTACHMENT0 ) + _index );

        // For the future: https://stackoverflow.com/a/17092208
        // It explains the difference between glFramebufferTexture and glFramebufferTexture2D, as well as a bit about cubemaps
        // TLDR: Only relevant for cubemap faces.

        gl::glNamedFramebufferTexture( m_frame_buffer_, attachment, _target->get_texture(), 0 );

        if( m_render_targets_.size() < _index )
            m_render_targets_.resize( _index );

        m_render_targets_[ _index ] = _target;

        if( m_depth_target_ )
            _target->m_rel_scale_ = static_cast< cVector2f >( _target->m_resolution_ ) / m_depth_target_->m_resolution_;

    } // Bind

    void cFrame_Buffer::Bind( const cShared_ptr< cDepth_Target >& _depth_target, const bool _force )
    {
        if( m_depth_target_ == _depth_target && !_force )
            return;

        if( m_depth_target_ )
            UnbindDepthTarget();

        m_depth_target_ = _depth_target;
        m_depth_type_   = _depth_target->m_buffer_type_;

        for( auto& render_target : m_render_targets_ )
        {
            if( render_target )
                render_target->m_rel_scale_ = static_cast< cVector2f >( render_target->m_resolution_ ) / m_depth_target_->m_resolution_;
        }

        gl::glNamedFramebufferRenderbuffer( m_frame_buffer_, m_depth_type_, gl::GL_RENDERBUFFER, m_depth_target_->m_buffer_ );
    } // Bind
    
    void cFrame_Buffer::BindVertexBuffer( size_t _binding, const cDynamic_Buffer& _buffer )
    {
        auto& buffer = static_cast< cUnsafe_Buffer& >( _buffer.GetBuffer() );
        
        // TODO: Vertex buffer binding.
        
        gl::glBindVertexBuffer( _binding, buffer.get_buffer().buffer, 0, 0 );
        
    }

    void cFrame_Buffer::UnbindVertexBuffers()
    {
    }

    void cFrame_Buffer::UnbindRenderTargetAt( const size_t _index )
    {
        // The GL_COLOR_ATTACKMENTX are after one another. Making this safe.
        const auto attachment = static_cast< gl::GLenum >( static_cast< size_t >( gl::GL_COLOR_ATTACHMENT0 ) + _index );

        gl::glNamedFramebufferTexture( m_frame_buffer_, attachment, 0, 0 );

        auto& target = m_render_targets_[ _index ];
        target->m_rel_scale_ = kOne;
        target = nullptr;
    } // UnbindRenderTarget

    void cFrame_Buffer::UnbindDepthTarget()
    {
        
        // If the render target is unbound return all render target scales to normal.
        for( auto& render_target : m_render_targets_ )
        {
            if( render_target )
                render_target->m_rel_scale_ = kOne;
        }

        m_depth_target_ = nullptr;
        gl::glNamedFramebufferRenderbuffer( m_frame_buffer_, m_depth_type_, gl::GL_RENDERBUFFER, 0 );
    } // UnbindDepthTarget

    void cFrame_Buffer::Resize( const cVector2u32& _new_resolution )
    {
        gl::gldraw
        destroy();

        const auto render_targets = m_render_targets_;
        for( const auto& render_target : render_targets )
            render_target->Resize( _new_resolution );
        
        create();

        // Rebind all targets.
        m_render_targets_.clear();
        m_render_targets_.resize( render_targets.size() );
        for( size_t i = 0; i < render_targets.size(); ++i )
            Bind( i, render_targets[ i ] );
    } // Resize
} // sk::Graphics::Rendering