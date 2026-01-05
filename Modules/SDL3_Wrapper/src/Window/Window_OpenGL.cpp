// Window functions:

#ifdef SK_GRAPHICS_OPENGL

#include "Window.h"

#include <Memory/Tracker/Tracker.h>
#include <Platform/Platform_Base.h>

#include <SDL3/SDL.h>

namespace sk::Platform
{
    cSDL_Window::cSDL_Window( const std::string& _name, const cVector2u32& _resolution, const uint8_t _flags )
    : m_visible_( false )
    , m_size_( _resolution )
    , m_aspect_ratio_( Vector2::Aspect( _resolution ) )
    {
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, SK_OPENGL_MAJOR_VERSION );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, SK_OPENGL_MINOR_VERSION );

        const SDL_WindowFlags flags = _flags | SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        m_window_ = SDL_CreateWindow( _name.c_str(), static_cast< int >( _resolution.x ), static_cast< int >( _resolution.y ), flags );
        SK_ERR_IF( m_window_ == nullptr,
            TEXT( "ERROR: Unable to create a window.\n Reason: {}", SDL_GetError() ) )
    } // cSDL_Window

    // TODO: Move to namespace instead of inside of class.
    void* create_context( const cSDL_Window& _window )
    {
        return SDL_GL_CreateContext( _window.GetWindow() );
    } // create_context

    function_ptr_t get_proc_address( const char* _name )
    {
        return SDL_GL_GetProcAddress( _name );
    } // getProcAddress
    
} // sk::Platform::

#endif // SK_GRAPHICS_OPENGL

