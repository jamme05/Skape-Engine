/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Frame_Buffer.h"

#include <sk/Graphics/Renderer_Impl.h>
#include <sk/Graphics/Buffer/Unsafe_Buffer.h>
#include <sk/Graphics/Rendering/Depth_Target.h>
#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Graphics/Rendering/Scissor.h>
#include <sk/Graphics/Rendering/Viewport.h>
#include <sk/Graphics/Utils/Shader_Reflection.h>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

using namespace sk::Graphics::Rendering;

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

#ifdef DEBUG
        if( const auto itr = std::ranges::find_if( m_render_targets_,
            []( auto& _value ){ return _value == nullptr; } ); itr != m_render_targets_.end() )
        {
            SK_WARNING( sk::Severity::kGraphics, "You have an uninitialized render target!!" )
            SK_BREAK;
        }
#endif // DEBUG
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
    if( m_render_targets_.empty() )
    {
        // TODO: Actually validate the clear.
        gl::glClear( gl::ClearBufferMask::GL_COLOR_BUFFER_BIT | gl::ClearBufferMask::GL_DEPTH_BUFFER_BIT );
        return;
    }
    
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

    const auto attachment = static_cast< gl::GLenum >( static_cast< size_t >( gl::GL_COLOR_ATTACHMENT0 ) + _index );

    if( m_render_targets_[ _index ] == _target && !_force )
    {
        gl::glNamedFramebufferTexture( m_frame_buffer_, attachment, _target->get_texture_object(), 0 );
        return;
    }

    if( m_render_targets_[ _index ] )
        UnbindRenderTargetAt( _index );

    // The GL_COLOR_ATTACKMENTX are after one another. Making this safe.

    // For the future: https://stackoverflow.com/a/17092208
    // It explains the difference between glFramebufferTexture and glFramebufferTexture2D, as well as a bit about cubemaps
    // TLDR: Only relevant for cubemap faces.

    gl::glNamedFramebufferTexture( m_frame_buffer_, attachment, _target->get_texture_object(), 0 );

    if( m_render_targets_.size() < _index )
        m_render_targets_.resize( _index );

    m_render_targets_[ _index ] = _target;

    if( m_depth_target_ )
        _target->m_rel_scale_ = static_cast< cVector2f >( _target->m_resolution_ ) / m_depth_target_->m_resolution_;

} // Bind

void cFrame_Buffer::Bind( const cShared_ptr< cDepth_Target >& _depth_target, const bool _force )
{
    if( m_depth_target_ == _depth_target && !_force )
    {
        gl::glNamedFramebufferRenderbuffer( m_frame_buffer_, m_depth_type_, gl::GL_RENDERBUFFER, m_depth_target_->m_buffer_ );
        return;
    }

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

auto cFrame_Buffer::GetRenderTarget( const size_t _index ) const -> const cShared_ptr< cRender_Target >&
{
    return m_render_targets_[ _index ];
}

auto cFrame_Buffer::GetDepthTarget() const -> const cShared_ptr<cDepth_Target>&
{
    return m_depth_target_;
}

void cFrame_Buffer::BindVertexBuffer( const size_t _binding, const cDynamic_Buffer* _buffer )
{
    gl::GLuint  buffer_object;
    gl::GLsizei stride = 0;
    if( _buffer )
    {
        auto& buffer = static_cast< cUnsafe_Buffer& >( _buffer->GetBuffer() );
        buffer.Upload( false );
        buffer_object = buffer.get_buffer().object;
        stride = static_cast< gl::GLsizei >( buffer.GetStride() );
    }
    else
    {
        SK_WARNING( sk::Severity::kGraphics, "A default vertex buffer is currently not supported. Do check if there's any alternative vertex buffer that can be used." )
        SK_BREAK;
        buffer_object = cGLRenderer::get().GetFallbackVertexBuffer().get_buffer().object;
    }
    
    if( m_bound_vertex_buffers_.size() < _binding )
        m_bound_vertex_buffers_.resize( _binding );
    
    gl::glVertexArrayVertexBuffer( m_vertex_array_,
        static_cast< gl::GLuint >( _binding ), buffer_object,
        0, stride
    );
    
    if( m_bound_vertex_buffers_.size() <= _binding )
        m_bound_vertex_buffers_.resize( _binding + 1 );
    
    m_bound_vertex_buffers_[ _binding ] = _buffer;
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
    gl::glBindBuffer( gl::GL_ELEMENT_ARRAY_BUFFER, buffer.get_buffer().object );
    gl::glBindVertexArray( 0 );
    
    m_bound_index_buffer_ = &_buffer;
}

void cFrame_Buffer::UnbindIndexBuffer() const
{
    gl::glBindVertexArray( m_vertex_array_ );
    gl::glBindBuffer( gl::GL_ELEMENT_ARRAY_BUFFER, 0 );
    gl::glBindVertexArray( 0 );
}

namespace 
{
    // From: https://en.cppreference.com/w/cpp/utility/variant/visit
    template< class... Ts >
    struct sVisitor : Ts... { using Ts::operator()...; };
} // ::

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
        auto& buffer = static_cast< const cUnsafe_Buffer& >( block.GetBuffer() );
        m_assigned_blocks_.emplace_back( block.m_binding_ );
        gl::glUniformBlockBinding( link.get_program(), static_cast< gl::GLuint >( block.m_binding_ ), static_cast< gl::GLuint >( block.m_binding_ ) );
        gl::glBindBufferBase( gl::GL_UNIFORM_BUFFER, static_cast< gl::GLuint >( block.m_binding_ ), buffer.get_buffer().object );
    }

    auto& buffers = _material.GetBuffers();
    for( const auto& [ buffer_info, buffer ] : buffers | std::views::values )
    {
        gl::glBindBufferBase( gl::GL_SHADER_STORAGE_BUFFER, buffer_info->binding, buffer ? static_cast< const cUnsafe_Buffer* >( buffer )->get_buffer().object : 0 );
    }
    m_bound_buffers_ = buffers.size();

    constexpr sVisitor visitor{
        []( const cShared_ptr< cRender_Target >& _render_target ) -> std::optional< std::pair< gl::GLenum, gl::GLuint > >
        {
            SK_BREAK_RET_IF( sk::Severity::kGraphics,
                _render_target == nullptr, "Warning: This shouldn't happen", std::nullopt )

            return std::make_pair( gl::GL_TEXTURE_2D, _render_target->get_texture_object() );
        },
        []( const cAsset_Ref< Assets::cTexture >& _texture ) -> std::optional< std::pair< gl::GLenum, gl::GLuint > >
        {
            SK_BREAK_RET_IFN( sk::Severity::kGraphics,
                _texture.IsValid(), "Warning: This shouldn't happen.", std::nullopt )

            if( !_texture.IsLoaded() )
                return std::nullopt;

            return std::make_pair( gl::GL_TEXTURE_2D, _texture->get_texture_object().m_buffer_ );
        },
        []( auto& ) -> std::optional< std::pair< gl::GLenum, gl::GLuint > >
        {
            return std::nullopt;
        }
    };
    
    auto& textures = _material.GetTextures();
    for( uint_fast32_t i = 0; i < textures.size(); i++ )
    {
        const auto& [ _, sampler, texture ] = textures[ i ];

        gl::glActiveTexture( gl::GL_TEXTURE0 + i );
        if( auto res = std::visit( visitor, texture ); res.has_value() )
            gl::glBindTexture( res->first, res->second );
        else
            gl::glBindTexture( gl::GL_TEXTURE_2D, 0 );
        gl::glUniform1i( sampler->location, static_cast< gl::GLint >( i ) );
    }
    m_bound_textures_ = textures.size();

    if( _material.GetDepthTest() != Assets::cMaterial::eDepthTest::kDisabled )
    {
        gl::glEnable( gl::GL_DEPTH_TEST );
        gl::GLenum depth_method = gl::GL_NEVER;
        switch( _material.GetDepthTest() )
        {
        case Assets::cMaterial::eDepthTest::kDisabled:     break;
        case Assets::cMaterial::eDepthTest::kNever:        depth_method = gl::GL_NEVER;    break;
        case Assets::cMaterial::eDepthTest::kAlways:       depth_method = gl::GL_ALWAYS;   break;
        case Assets::cMaterial::eDepthTest::kLess:         depth_method = gl::GL_LESS;     break;
        case Assets::cMaterial::eDepthTest::kLessEqual:    depth_method = gl::GL_LEQUAL;   break;
        case Assets::cMaterial::eDepthTest::kGreater:      depth_method = gl::GL_GREATER;  break;
        case Assets::cMaterial::eDepthTest::kGreaterEqual: depth_method = gl::GL_GEQUAL;   break;
        case Assets::cMaterial::eDepthTest::kEqual:        depth_method = gl::GL_EQUAL;    break;
        case Assets::cMaterial::eDepthTest::kNotEqual:     depth_method = gl::GL_NOTEQUAL; break;
        }
        gl::glDepthFunc( depth_method );
    }
    else
        gl::glDisable( gl::GL_DEPTH_TEST );


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

    for( size_t i = 0; i < m_bound_textures_; i++ )
    {
        gl::glActiveTexture( gl::GL_TEXTURE0 + i );
        gl::glBindTexture( gl::GL_TEXTURE_2D, 0 );
    }

    for( gl::GLuint i = 0; i < m_bound_buffers_; i++ )
        gl::glBindBufferBase( gl::GL_SHADER_STORAGE_BUFFER, i, 0 );

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
        render_target->Resize( _new_resolution, true );
    
    create( m_frame_buffer_ == 0 );

    m_depth_target_->Resize( _new_resolution );

    // Rebind all targets.
    Bind( m_depth_target_ );
    m_render_targets_.clear();
    m_render_targets_.resize( render_targets.size() );
    for( size_t i = 0; i < render_targets.size(); ++i )
        Bind( i, render_targets[ i ] );
} // Resize
