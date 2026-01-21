// Window functions:

#ifdef SK_GRAPHICS_VULKAN

#include "Window.h"

#include <Memory/Tracker/Tracker.h>
#include <Platform/Platform_Base.h>

#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

namespace sk::Platform
{
    cSDL_Window::cSDL_Window( const std::string& _name, const cVector2u32& _resolution, const uint8_t _flags )
    : m_visible_( false )
    , m_size_( _resolution )
    , m_aspect_ratio_( Vector2::Aspect( _resolution ) )
    {
        const SDL_WindowFlags flags = _flags | SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        m_window_ = SDL_CreateWindow( _name.c_str(), static_cast< int >( _resolution.x ), static_cast< int >( _resolution.y ), flags );
        SK_ERR_IF( m_window_ == nullptr,
            TEXT( "ERROR: Unable to create a window.\n Reason: {}", SDL_GetError() ) )
    } // cSDL_Window

    function_ptr_t get_proc_address( const char* _name )
    {
        return SDL_GL_GetProcAddress( _name );
    } // getProcAddress
    
    std::pair< uint32_t, const char* const* > get_extensions()
    {
        Uint32 extension_count;
        auto names = SDL_Vulkan_GetInstanceExtensions( &extension_count );
        
        return std::make_pair( extension_count, names );
    }
    
    void create_surface( cSDL_Window* _window )
    {
        // SDL_Vulkan_CreateSurface(  )
    }
    
} // sk::Platform::

#endif // SK_GRAPHICS_OPENGL

