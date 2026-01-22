/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Window.h"

#include <framework/App.h>
#include <sk/Skape_Main.h>
#include <sk/Input/Input.h>
#include <sk/Platform/Time.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

// SDL Functions:
SDL_AppResult SDL_AppInit( void** _app_state, int _argc, char** _argv )
{
    SK_ERR_IFN( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ),
        "Unable to initialize SDL" )

    sk::App::startup( _argc, _argv );

    SK_ERR_IF( std::atexit( &SDL_Quit ),
        "ERROR: Unable to add SDL_Quit to exit.")
    
    // Using the app as the app state. Maybe useful?
    *_app_state = cApp::getPtr();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate( void* _app_state )
{
    if( const auto app = cApp::getRunningInstance() )
    {
        app->run();

        return SDL_APP_CONTINUE;
    }
    return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent( void* _app_state, SDL_Event* _event )
{
    // TODO: Resize event.
    return sk::Platform::cSDL_Window::handle_event( _event );
}

void SDL_AppQuit( void* _app_state, SDL_AppResult _result )
{
    sk::App::shutdown();
}

// Window functions:
namespace sk::Platform
{
    // TODO: Assign the main window.
    cSDL_Window* cSDL_Window::g_main_window_ = nullptr;
    
    namespace 
    {
        std::map< SDL_WindowID, cSDL_Window* > window_map;
        
        cVector2f prev_mouse_position = {};
    } // ::

    SDL_AppResult cSDL_Window::handle_event( void* _event )
    {
        switch( const auto& event = *static_cast< SDL_Event* >( _event ); event.type )
        {
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_KEY_DOWN: return handle_event( event.key );
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN: return handle_event( event.gbutton );
        case SDL_EVENT_MOUSE_MOTION: return handle_event( event.motion );
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_BUTTON_DOWN: return handle_event( event.button );
        case SDL_EVENT_WINDOW_RESIZED: return handle_event( event.window );
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
        default:
            break;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult cSDL_Window::handle_event( const SDL_WindowEvent& _event )
    {
        const auto itr = window_map.find( _event.windowID );
        
        switch( _event.type )
        {
        case SDL_EVENT_WINDOW_RESIZED: itr->second->resize( cVector2u32{ _event.data1, _event.data2 } );
        default: break;
        }
        
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult cSDL_Window::handle_event( const SDL_KeyboardEvent& _event )
    {
        // Bit shift values required to convert the flags from SDL to my enum type.
        constexpr auto ctrl_shift   = 6;
        constexpr auto alt_shift    = 8;
        constexpr auto caps_shift   = 13;
        constexpr auto scroll_shift = 15;
        constexpr auto os_shift     = 10;
        
        Input::sKeyboardEvent event;
        event.key = _event.key;
        event.mods = {
            .shift       = static_cast< uint8_t >(   _event.mod & SDL_KMOD_SHIFT  ),
            .ctrl        = static_cast< uint8_t >( ( _event.mod & SDL_KMOD_CTRL   ) >> ctrl_shift   ),
            .alt         = static_cast< uint8_t >( ( _event.mod & SDL_KMOD_ALT    ) >> alt_shift    ),
            .caps_lock   = static_cast< uint8_t >( ( _event.mod & SDL_KMOD_CAPS   ) >> caps_shift   ),
            .scroll_lock = static_cast< uint8_t >( ( _event.mod & SDL_KMOD_SCROLL ) >> scroll_shift ),
            .os_down     = static_cast< uint8_t >( ( _event.mod & SDL_KMOD_GUI    ) >> os_shift     ),
        };
        // SDL doesn't have multiple repeats so stick with one or zero. It should be enough for pretty much all use cases.
        // In the case a user would need more they can implement something outside of this.
        event.repeat = _event.repeat ? 1 : 0;

        const auto event_type = _event.down ? Input::eInputType::kKey_Down : Input::eInputType::kKey_Up;

        // If false Continue. Otherwise, quit.
        return ( Input::input_event( event_type, event ) ) ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
    }

    SDL_AppResult cSDL_Window::handle_event( const SDL_GamepadButtonEvent& _event )
    {
        // TODO: Gamepad input events
        return SDL_APP_CONTINUE;
    } // handle_event

    SDL_AppResult cSDL_Window::handle_event( const SDL_MouseMotionEvent& _event )
    {
        const auto window     = SDL_GetWindowFromID( _event.windowID );
        const auto event_type = SDL_GetWindowRelativeMouseMode( window ) ? Input::eInputType::kMouseRelative : Input::eInputType::kMouseAbsolute;
        
        // TODO: Have some way to identify the window on our events.
        Input::sMouseEvent event;
        event.analog = Input::eAnalog::kMouse;
        event.previous_position = prev_mouse_position;
        // We need it to be conservative due to SDL never having it return with a relative of 0
        event.relative          = cVector2f
        {
            Math::abs( _event.yrel ) < 2 ? 0 : _event.yrel,
            Math::abs( _event.xrel ) < 2 ? 0 : _event.xrel
        };
        event.current_position  = cVector2f{ _event.y, _event.x };
        prev_mouse_position     = event.current_position;
        event.button            = 0;
        event.presses           = 0;
        
        return ( Input::input_event( event_type, event ) ) ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
    } // handle_event
    
    SDL_AppResult cSDL_Window::handle_event( const SDL_MouseButtonEvent& _event )
    {
        const auto event_type = ( _event.type == SDL_EVENT_MOUSE_BUTTON_UP ) ? Input::kMouse_Up : Input::kMouse_Down;
        
        Input::sMouseEvent event;
        event.analog = Input::eAnalog::kNone;
        event.previous_position = prev_mouse_position;
        event.current_position  = prev_mouse_position;
        event.relative          = cVector2f{};
        event.button            = _event.button;
        event.presses           = _event.clicks;
        
        return ( Input::input_event( event_type, event ) ) ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
    }

    void cSDL_Window::add_self()
    {
        const auto id = SDL_GetWindowID( m_window_ );
        window_map[ id ] = this;
    }

    void cSDL_Window::remove_self() const
    {
        const auto id = SDL_GetWindowID( m_window_ );
        window_map.erase( id );
    }

    void cSDL_Window::resize( const cVector2u32 _new_resolution )
    {
        m_size_ = _new_resolution;
        m_resized_on_frame_ = Time::Frame;
    }

    iWindow* CreateWindow( const std::string& _name, const cVector2u32& _size )
    {
        return SK_SINGLE( cSDL_Window, _name, _size );
    } // create_window
    
    void DestroyWindow( iWindow* _window )
    {
        SK_DELETE( _window );
    }

    iWindow* GetMainWindow()
    {
        return cSDL_Window::g_main_window_;
    } // GetMainWindow
    
    bool cSDL_Window::SetVisibility( const bool _visible ) const
    {
        if( _visible )
            return SDL_ShowWindow( m_window_ );
        return SDL_HideWindow( m_window_ );
    } // SetVisibility

    bool cSDL_Window::GetVisibility() const
    {
        return m_visible_;
    } // GetVisibility

    cVector2u32 cSDL_Window::GetResolution() const
    {
        return m_size_;
    } // GetSize

    float cSDL_Window::GetAspectRatio() const
    {
        return m_aspect_ratio_;
    } // GetAspectRatio
    
    void cSDL_Window::SetMouseCapture( const bool _capture )
    {
        // TODO: Add more handling.
        SDL_SetWindowRelativeMouseMode( m_window_, _capture );
    }
} // sk::Platform::
