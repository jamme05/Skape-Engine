// Window functions:

#include "Window.h"

#include <Memory/Tracker/Tracker.h>
#include <Platform/Platform_Base.h>

#include <SDL3/SDL.h>

#if defined( SK_GRAPHICS_OPENGL )

namespace sk::Platform
{
    cSDL_Window::cSDL_Window( const std::string& _name, const cVector2i& _size, uint8_t _flags )
    {
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

        const SDL_WindowFlags flags = _flags | SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        m_window_ = SDL_CreateWindow( _name.c_str(), _size.x, _size.y, flags );
        SK_ERR_IF( m_window_ == nullptr, "Unable to create a window." )
    } // cSDL_Window

    cSDL_Window::~cSDL_Window()
    {
        SDL_DestroyWindow( m_window_ );
    } // ~cSDL_Window

    void* cSDL_Window::create_context()
    {
        return SDL_GL_CreateContext( m_window_ );
    } // create_context

    function_ptr_t getProcAddress( const char* _name )
    {
        return SDL_GL_GetProcAddress( _name );
    } // getProcAddress
    
} // sk::Platform::

#endif // SK_GRAPHICS_OPENGL

