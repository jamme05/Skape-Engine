

#include "Shader_Link.h"

#include <sk/Assets/Material.h>
#include <sk/Graphics/Utils/Shader_Reflection.h>

#include <glbinding/gl/functions.h>

sk::Graphics::Utils::cShader_Link::cShader_Link(
    const cShared_ptr< cAsset_Meta >& _vertex_shader,
    const cShared_ptr< cAsset_Meta >& _fragment_shader )
: m_vertex_shader_  ( nullptr )
, m_fragment_shader_( nullptr )
{
    const auto event = CreateEvent( this, &cShader_Link::on_shader_changed );
    m_vertex_shader_.on_changed   += event;
    m_fragment_shader_.on_changed += event;
    
    m_program_ = gl::glCreateProgram();
    
    m_vertex_shader_   = _vertex_shader;
    m_fragment_shader_ = _fragment_shader;
    
    Complete();
}

sk::Graphics::Utils::cShader_Link::cShader_Link( const cShader_Link& _other )
{
    m_program_ = gl::glCreateProgram();
    
    m_vertex_shader_   = _other.m_vertex_shader_;
    m_fragment_shader_ = _other.m_fragment_shader_;
    
    Complete();
}

sk::Graphics::Utils::cShader_Link::cShader_Link( cShader_Link&& _other ) noexcept
{
    m_vertex_shader_   = std::move( _other.m_vertex_shader_ );
    m_fragment_shader_ = std::move( _other.m_fragment_shader_ );

    m_program_ = _other.m_program_;
    _other.m_program_ = 0;
}

sk::Graphics::Utils::cShader_Link::~cShader_Link()
{
    gl::glDeleteProgram( m_program_ );
}

sk::Graphics::Utils::cShader_Link& sk::Graphics::Utils::cShader_Link::operator=( const cShader_Link& _other )
{
    if( m_program_ != 0 )
        gl::glDeleteProgram( m_program_ );

    m_program_ = gl::glCreateProgram();

    m_vertex_shader_   = _other.m_vertex_shader_;
    m_fragment_shader_ = _other.m_fragment_shader_;

    Complete();

    return *this;
}

sk::Graphics::Utils::cShader_Link& sk::Graphics::Utils::cShader_Link::operator=( cShader_Link&& _other ) noexcept
{
    if( m_program_ != 0 )
        gl::glDeleteProgram( m_program_ );

    m_vertex_shader_   = std::move( _other.m_vertex_shader_ );
    m_fragment_shader_ = std::move( _other.m_fragment_shader_ );

    m_program_ = _other.m_program_;
    _other.m_program_ = 0;

    return *this;
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

void sk::Graphics::Utils::cShader_Link::Complete()
{
    m_vertex_shader_.WaitUntilLoaded();
    m_fragment_shader_.WaitUntilLoaded();
    
    link_shaders();
    
    m_reflection_  = sk::make_shared< cShader_Reflection >( m_program_ );
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
}

void sk::Graphics::Utils::cShader_Link::link_shaders()
{
    if( m_is_linked_ )
        unlink_shaders();
    
    // TODO: Unload the shader once they've been linked.
    
    gl::glAttachShader( m_program_, m_vertex_shader_->m_shader_ );
    gl::glAttachShader( m_program_, m_fragment_shader_->m_shader_ );
    gl::glLinkProgram( m_program_ );
    
    m_has_updated_ = true;
    m_is_linked_   = true;
}

void sk::Graphics::Utils::cShader_Link::unlink_shaders()
{
    gl::glDetachShader( m_program_, m_vertex_shader_->m_shader_ );
    gl::glDetachShader( m_program_, m_fragment_shader_->m_shader_ );
    gl::glLinkProgram( m_program_ );
    
    m_is_linked_ = false;
}
