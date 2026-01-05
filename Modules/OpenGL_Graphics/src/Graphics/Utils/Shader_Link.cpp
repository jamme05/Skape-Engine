

#include "Shader_Link.h"

#include <glbinding/gl/functions.h>

#include "Shader_Reflection.h"
#include "Assets/Material.h"

sk::Graphics::Utils::cShader_Link::cShader_Link(
    const cShared_ptr< cAsset_Meta >& _vertex_shader,
    const cShared_ptr< cAsset_Meta >& _fragment_shader )
: m_vertex_shader_  ( nullptr )
, m_fragment_shader_( nullptr )
{
    m_vertex_shader_.on_changed   += CreateEvent( this, &on_shader_changed );
    m_fragment_shader_.on_changed += CreateEvent( this, &on_shader_changed );
    
    m_vertex_shader_   = _vertex_shader;
    m_fragment_shader_ = _fragment_shader;
    
    m_program_ = gl::glCreateProgram();
}

sk::Graphics::Utils::cShader_Link::cShader_Link( const cShader_Link& _other )
{
    m_program_ = gl::glCreateProgram();
    
    m_vertex_shader_   = _other.m_vertex_shader_;
    m_fragment_shader_ = _other.m_fragment_shader_;
}

bool sk::Graphics::Utils::cShader_Link::HasUpdated() const
{
    // TODO: Make this function not const
    const bool tmp = m_has_updated_;
    const_cast< bool& >( m_has_updated_ ) = false;
    return tmp;
}

bool sk::Graphics::Utils::cShader_Link::IsValid() const
{
    return m_vertex_shader_.IsValid() && m_fragment_shader_.IsValid();
}

bool sk::Graphics::Utils::cShader_Link::IsReady() const
{
    return m_fragment_shader_.IsLoaded() && m_vertex_shader_.IsLoaded();
}

void sk::Graphics::Utils::cShader_Link::WaitUntilReady() const
{
    m_vertex_shader_.WaitUntilLoaded();
    m_fragment_shader_.WaitUntilLoaded();
}

auto sk::Graphics::Utils::cShader_Link::GetVertexShader() const -> const Assets::cShader&
{
    return *m_vertex_shader_;
}

auto sk::Graphics::Utils::cShader_Link::GetFragmentShader() const -> const Assets::cShader&
{
    return *m_fragment_shader_;
}

auto sk::Graphics::Utils::cShader_Link::GetReflection() const -> cShared_ptr< cShader_Reflection >
{
    return m_reflection_;
}

void sk::Graphics::Utils::cShader_Link::Use() const
{
    gl::glUseProgram( m_program_ );
}

void sk::Graphics::Utils::cShader_Link::on_shader_changed( const Assets::eEventType _event, cAsset_Ref< Assets::cShader >& )
{
    // The asset SHOULD never be unloaded.
    if( _event == Assets::eEventType::kUnload )
        return;
    
    if( IsReady() )
        link_shaders();
}

void sk::Graphics::Utils::cShader_Link::link_shaders()
{
    if( m_is_linked_ )
        unlink_shaders();
    
    // TODO: Unload the shader once they've been linked.
    
    gl::glAttachShader( m_program_, m_vertex_shader_->m_shader_ );
    gl::glAttachShader( m_program_, m_fragment_shader_->m_shader_ );
    gl::glLinkProgram( m_program_ );
    
    m_reflection_  = sk::make_shared< cShader_Reflection >( m_program_ );
    m_has_updated_ = true;
}

void sk::Graphics::Utils::cShader_Link::unlink_shaders()
{
    gl::glDetachShader( m_program_, m_vertex_shader_->m_shader_ );
    gl::glDetachShader( m_program_, m_fragment_shader_->m_shader_ );
    
    m_is_linked_ = false;
}
