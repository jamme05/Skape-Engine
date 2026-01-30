
#include <sk/Assets/Shader.h>
#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Graphics/Utils/RenderUtils.h>
#include <sk/Graphics/Utils/RenderUtils_Ex.h>
#include <sk/Graphics/Buffer/Buffer.h>

using namespace sk;
using namespace sk::Graphics::Utils;

// TODO: Add baked sources.
namespace
{
    constexpr std::string_view kCopyRTVertexSource =
        "#version 330 core\nlayout ( location = 0 ) in vec2 POSITION; out vec2 ScreenPos;void main(){ ScreenPos = POSITION; gl_Position = vec4( ( POSITION - vec2( 0.5, 0.5 ) ) * 2, 0, 1 ); }";
    constexpr std::string_view kCopyRTPixelSource =
        "#version 330 core\nin vec2 ScreenPos; out vec4 FragColor; uniform sampler2D Source;void main(){ FragColor = texture( Source, ScreenPos ).rgba; }";
    constexpr cVector2f kVertexArray[] = {
        { 0, 0 }, { 0, 1 }, { 1, 0 },
        { 1, 0 }, { 0, 1 }, { 1, 1 }
    };

    gl::GLuint copy_rt_program;
    gl::GLuint copy_rt_frame_buffer;
    gl::GLuint copy_rt_vertex_array;
    Graphics::cVertex_Buffer< cVector2f > screen_vertex_buffer;
} // ::

void Graphics::Utils::InitRenderUtils_Ex()
{
    // Init vertex buffer
    screen_vertex_buffer = Graphics::cVertex_Buffer< cVector2f >( "Copy RT Vertex Buffer", std::begin( kVertexArray ), std::end( kVertexArray ), true );

    // We're gonna write it from scratch to skip the abstraction overhead.
    copy_rt_program = gl::glCreateProgram();

    // Init shaders
    const auto vertex_shader = Assets::cShader{ Assets::cShader::eType::kVertex,   kCopyRTVertexSource.data(), kCopyRTVertexSource.size() };
    const auto pixel_shader  = Assets::cShader{ Assets::cShader::eType::kFragment, kCopyRTPixelSource .data(), kCopyRTPixelSource .size() };

    // Init program
    gl::glAttachShader( copy_rt_program, vertex_shader.get_shader_object() );
    gl::glAttachShader( copy_rt_program, pixel_shader .get_shader_object() );
    gl::glLinkProgram( copy_rt_program );

    // Init frame buffer and vao
    gl::glGenFramebuffers( 1, &copy_rt_frame_buffer );
    gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, copy_rt_frame_buffer );
    gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );

    gl::glGenVertexArrays( 1, &copy_rt_vertex_array );
    gl::glBindVertexArray( copy_rt_vertex_array );
    gl::glEnableVertexAttribArray( 0 );
    gl::glVertexAttribFormat( 0, 2, gl::GL_FLOAT, gl::GL_FALSE, 0 );
    gl::glVertexAttribBinding( 0, 0 );
    gl::glVertexArrayVertexBuffer( copy_rt_vertex_array, 0,
        static_cast< const cUnsafe_Buffer& >( screen_vertex_buffer.GetBuffer() ).get_buffer().object, 0, sizeof( cVector2f ) );
    gl::glBindVertexArray( 0 );
}

void Graphics::Utils::ShutdownRenderUtils_Ex()
{
    screen_vertex_buffer = {};
    gl::glDeleteVertexArrays( 1, &copy_rt_vertex_array );
    gl::glDeleteFramebuffers( 1, &copy_rt_frame_buffer );
    gl::glDeleteProgram( copy_rt_program );
}

void Graphics::Utils::CopyRenderTarget( const Rendering::cRender_Target& _source, const Rendering::cRender_Target& _destination )
{
    gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, copy_rt_frame_buffer );
    gl::glViewport( 0, 0, _destination.GetResolution().x, _destination.GetResolution().y );
    gl::glScissor( 0, 0, _destination.GetResolution().x, _destination.GetResolution().y );

    gl::glNamedFramebufferTexture( copy_rt_frame_buffer, gl::GL_COLOR_ATTACHMENT0, _destination.get_texture_object(), 0 );

    gl::glUseProgram( copy_rt_program );

    gl::glActiveTexture( gl::GL_TEXTURE0 );
    gl::glBindTexture( gl::GL_TEXTURE_2D, _source.get_texture_object() );
    gl::glUniform1i( 0, 0 );

    gl::glBindVertexArray( copy_rt_vertex_array );

    gl::glDrawArrays( gl::GL_TRIANGLES, 0, std::size( kVertexArray ) );

    gl::glBindTexture( gl::GL_TEXTURE_2D, 0 );
    gl::glNamedFramebufferTexture( copy_rt_frame_buffer, gl::GL_COLOR_ATTACHMENT0, 0, 0 );
    gl::glBindVertexArray( 0 );
    gl::glUseProgram( 0 );
    gl::glBindFramebuffer( gl::GL_FRAMEBUFFER, 0 );
}