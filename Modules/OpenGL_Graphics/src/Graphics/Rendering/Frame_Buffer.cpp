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
#include "Graphics/Utils/Shader_Reflection.h"

namespace sk::Graphics::Rendering
{
    cFrame_Buffer::cFrame_Buffer( const size_t _render_targets )
    : m_render_targets_( _render_targets )
    {
        SK_ERR_IF( _render_targets > 31, "ERROR: OpenGL doesn't support more than 31 render targets." )
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
        
        if( !m_render_targets_.empty() )
        {
            // TODO: Maybe limit the max number of render targets, to like... 8?
            static constexpr gl::GLenum kAttachments[] = {
                gl::GL_COLOR_ATTACHMENT0,  gl::GL_COLOR_ATTACHMENT1,  gl::GL_COLOR_ATTACHMENT2,  gl::GL_COLOR_ATTACHMENT3,  gl::GL_COLOR_ATTACHMENT4,
                gl::GL_COLOR_ATTACHMENT5,  gl::GL_COLOR_ATTACHMENT6,  gl::GL_COLOR_ATTACHMENT7,  gl::GL_COLOR_ATTACHMENT8,  gl::GL_COLOR_ATTACHMENT9,
                gl::GL_COLOR_ATTACHMENT10, gl::GL_COLOR_ATTACHMENT11, gl::GL_COLOR_ATTACHMENT12, gl::GL_COLOR_ATTACHMENT13, gl::GL_COLOR_ATTACHMENT14,
                gl::GL_COLOR_ATTACHMENT15, gl::GL_COLOR_ATTACHMENT16, gl::GL_COLOR_ATTACHMENT17, gl::GL_COLOR_ATTACHMENT18, gl::GL_COLOR_ATTACHMENT19,
                gl::GL_COLOR_ATTACHMENT20, gl::GL_COLOR_ATTACHMENT21, gl::GL_COLOR_ATTACHMENT22, gl::GL_COLOR_ATTACHMENT23, gl::GL_COLOR_ATTACHMENT24,
            };
            
            gl::glDrawBuffers( static_cast< gl::GLsizei >( m_render_targets_.size() ), kAttachments );
        }
    } // Begin

    void cFrame_Buffer::End()
    {
        gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );
    } // End

    void cFrame_Buffer::create( const bool _is_window_frame )
    {
        if( !_is_window_frame )
        {
            gl::glGenFramebuffers( 1, &m_frame_buffer_ );
            gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, m_frame_buffer_ );
            // TODO: Was I required to do anything here?
            gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );
        }
        else
            m_frame_buffer_ = 0;
        
        gl::glGenVertexArrays( 1, &m_vertex_array_ );
    } // create

    void cFrame_Buffer::destroy() const
    {
        gl::glDeleteVertexArrays( 1, &m_vertex_array_ );
        if( m_frame_buffer_ != 0 )
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
    
    void cFrame_Buffer::BindVertexBuffer( const size_t _binding, const cDynamic_Buffer& _buffer )
    {
        auto& buffer = static_cast< cUnsafe_Buffer& >( _buffer.GetBuffer() );
        buffer.Upload( false );
        
        if( m_bound_vertex_buffers_.size() < _binding )
            m_bound_vertex_buffers_.resize( _binding );
        
        gl::glBindVertexArray( m_vertex_array_ );
        gl::glBindVertexBuffer(
            static_cast< gl::GLuint >( _binding ), buffer.get_buffer().buffer,
            0, static_cast< gl::GLsizei >( buffer.GetStride() )
        );
        gl::glBindVertexArray( 0 );
        
        if( m_bound_vertex_buffers_.size() <= _binding )
            m_bound_vertex_buffers_.resize( _binding + 1 );
        
        m_bound_vertex_buffers_[ _binding ] = &_buffer;
    }

    void cFrame_Buffer::UnbindVertexBuffers()
    {
        gl::glBindVertexArray( m_vertex_array_ );
        for( size_t i = 0; i < m_bound_vertex_buffers_.size(); i++ )
            gl::glBindVertexBuffer( static_cast< gl::GLuint >( i ), 0, 0, 0 );
        gl::glBindVertexArray( 0 );
        
        m_bound_vertex_buffers_.clear();
    }

    void cFrame_Buffer::BindIndexBuffer( const cDynamic_Buffer& _buffer )
    {
        auto& buffer = static_cast< cUnsafe_Buffer& >( _buffer.GetBuffer() );
        buffer.Upload( false );
        // TODO: Validate the buffer.
        
        gl::glBindVertexArray( m_vertex_array_ );
        gl::glBindBuffer( gl::GL_ELEMENT_ARRAY_BUFFER, buffer.get_buffer().buffer );
        gl::glBindVertexArray( 0 );
        
        m_bound_index_buffer_ = &_buffer;
    }

    void cFrame_Buffer::UnbindIndexBuffer() const
    {
        gl::glBindVertexArray( m_vertex_array_ );
        gl::glBindBuffer( gl::GL_ELEMENT_ARRAY_BUFFER, 0 );
        gl::glBindVertexArray( 0 );
    }

    bool cFrame_Buffer::UseMaterial( const Assets::cMaterial& _material )
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics, !_material.IsReady(),
            "Error: Material isn't ready yet.", false )
        
        auto& link = _material.GetShaderLink();
        
        link.Use();
        
        auto reflection = link.GetReflection();
        auto& attributes = reflection->GetAttributes();

        gl::glBindVertexArray( m_vertex_array_ );
        
        // TODO: Make into a int for loop
        gl::GLuint index = 0;
        for( auto& attribute : attributes )
        {
            gl::glEnableVertexAttribArray( index );
            // TODO: Support normalizing in the future.
            gl::glVertexAttribFormat( index, attribute.components, attribute.gl_type, false, 0 );
            gl::glVertexAttribBinding( index, index );
            
            ++index;
        }
        m_attribute_count_ = index;
        
        gl::glBindVertexArray( 0 );
            
        for( auto& block : _material.GetBlocks() | std::views::values )
        {
            m_assigned_blocks_.emplace_back( block.m_binding_ );
            gl::glUniformBlockBinding( link.get_program(), static_cast< gl::GLuint >( block.m_binding_ ), static_cast< gl::GLuint >( block.m_binding_ ) );
            gl::glBindBufferBase( gl::GL_UNIFORM_BUFFER, static_cast< gl::GLuint >( block.m_binding_ ), block.m_buffer_.get_buffer().buffer );
        }

        return true;
    }

    void cFrame_Buffer::ResetMaterial()
    {
        gl::glBindVertexArray( m_vertex_array_ );
        
        for( uint32_t i = 0; i < m_attribute_count_; i++ )
            gl::glDisableVertexAttribArray( i );
        m_attribute_count_ = 0;
        
        gl::glBindVertexArray( 0 );
        
        for( const auto& block : m_assigned_blocks_ )
            gl::glBindBufferBase( gl::GL_UNIFORM_BUFFER, static_cast< gl::GLuint >( block ), 0 );
        
        gl::glUseProgram( 0 );
        
        m_assigned_blocks_.clear();
    }

    bool cFrame_Buffer::DrawIndexed( const size_t _start, size_t _end ) const
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics, _start > m_bound_index_buffer_->GetSize(),
            "Error: Offset is greater than index buffer size!", false )
        
        if( _end == std::numeric_limits< size_t >::max() )
            _end = m_bound_index_buffer_->GetSize();
        else
        {
            SK_BREAK_RET_IF( sk::Severity::kGraphics, _end > m_bound_index_buffer_->GetSize(),
                "Error: End is outside of the index buffers range.", false )
        }

        const auto size = _end - _start;

        gl::GLenum type;
        switch( m_bound_index_buffer_->GetItemType()->hash )
        {
        case kTypeId< uint16_t >: type = gl::GL_UNSIGNED_SHORT; break;
        case kTypeId< uint32_t >: type = gl::GL_UNSIGNED_INT;   break;
        default: type = gl::GL_INVALID_ENUM; break;
        }
        
        SK_BREAK_RET_IF( sk::Severity::kGraphics, type == gl::GL_INVALID_ENUM,
            TEXT( "Error: Invalid index buffer type Has to be one of uint16_t or uint32_t. But {} was provided instead!",
                m_bound_index_buffer_->GetItemType()->name ), false )
        
        gl::glBindVertexArray( m_vertex_array_ );
        gl::glDrawElements( gl::GL_TRIANGLES, static_cast< gl::GLsizei >( size ), type, nullptr );
        gl::glBindVertexArray( 0 );
        
        return true;
    }

    bool cFrame_Buffer::DrawAuto() const
    {
        if( m_bound_index_buffer_ != nullptr )
            return DrawIndexed();
        
        if( m_bound_vertex_buffers_.empty() )
            return false;
        
        gl::glBindVertexArray( m_vertex_array_ );
        gl::glDrawArrays( gl::GL_TRIANGLES, 0, static_cast< gl::GLsizei >( m_bound_vertex_buffers_[ 0 ]->GetSize() ) );
        gl::glBindVertexArray( 0 );
        
        return true;
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
        destroy();

        const auto render_targets = m_render_targets_;
        for( const auto& render_target : render_targets )
            render_target->Resize( _new_resolution );
        
        create( m_frame_buffer_ == 0 );

        // Rebind all targets.
        m_render_targets_.clear();
        m_render_targets_.resize( render_targets.size() );
        for( size_t i = 0; i < render_targets.size(); ++i )
            Bind( i, render_targets[ i ] );
    } // Resize
} // sk::Graphics::Rendering