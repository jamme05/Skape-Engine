// Window functions:

#ifdef SK_GRAPHICS_OPENGL

#include <glbinding/glbinding.h>

#include "Window.h"

#include <Memory/Tracker/Tracker.h>
#include <Platform/Platform_Base.h>

#include <SDL3/SDL.h>

#include "Graphics/Rendering/Window_Context.h"

using namespace sk::Platform;

cSDL_Window::cSDL_Window( const std::string& _name, const cVector2u32& _resolution, const uint8_t _flags )
: m_visible_( false )
, m_size_( _resolution )
, m_aspect_ratio_( Vector2::Aspect( _resolution ) )
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, SK_OPENGL_MAJOR_VERSION );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, SK_OPENGL_MINOR_VERSION );

    const SDL_WindowFlags flags = _flags | SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE;
    m_window_ = SDL_CreateWindow( _name.c_str(), static_cast< int >( _resolution.x ), static_cast< int >( _resolution.y ), flags );
    SK_ERR_IF( m_window_ == nullptr,
        TEXT( "ERROR: Unable to create a window.\n Reason: {}", SDL_GetError() ) )
} // cSDL_Window


cSDL_Window::~cSDL_Window()
{
    for( const auto& context : m_contexts_ )
        SDL_GL_DestroyContext( context );
    
    SDL_DestroyWindow( m_window_ );
} // ~cSDL_Window

void cSDL_Window::Init()
{
    m_window_context_ = std::make_unique< Graphics::Rendering::cWindow_Context >( *this );
}

auto cSDL_Window::GetWindowContext() const -> Graphics::Rendering::cWindow_Context&
{
    return *m_window_context_;
}

void cSDL_Window::SwapBuffers()
{
    // Not needed for OpenGL as it only supports the default frame buffer for the window.
    // m_window_context_->Swap();
    
    SDL_GL_SwapWindow( m_window_ );
}

void cSDL_Window::PushContext()
{
    m_contexts_.emplace_back( SDL_GL_CreateContext( m_window_ ) );
}

void cSDL_Window::PopContext()
{
    SDL_GL_DestroyContext( m_contexts_.back() );
    m_contexts_.pop_back();
}

function_ptr_t sk::Platform::get_proc_address( const char* _name )
{
    return SDL_GL_GetProcAddress( _name );
} // getProcAddress

#endif // SK_GRAPHICS_OPENGL

