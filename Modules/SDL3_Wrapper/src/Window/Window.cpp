/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Window.h"

#include <App.h>
#include <Skape_Main.h>
#include <Input/Input.h>

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
    return sk::Platform::cSDL_Window::handle_event( _event );
}

void SDL_AppQuit( void* _app_state, SDL_AppResult _result )
{
    sk::App::shutdown();
}

// Window functions:
namespace sk::Platform
{
    SDL_AppResult cSDL_Window::handle_event( void* _event )
    {
        switch( const auto& event = *static_cast< SDL_Event* >( _event ); event.type )
        {
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_KEY_DOWN: return handle_event( event.key );
        default:
            break;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult cSDL_Window::handle_event( const SDL_KeyboardEvent& _event )
    {
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
        event.repeat = static_cast< bool >( event.repeat );

        const auto event_type = _event.down ? Input::eInputType::kKey_Down : Input::eInputType::kKey_Up;

        // If false Continue. Otherwise, quit.
        return ( Input::input_event( event_type, event ) ) ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
    }

    iWindow* create_window( const std::string& _name, const cVector2u& _size )
    {
        return SK_SINGLE( cSDL_Window, _name, static_cast< cVector2i >( _size ) );
    } // create_window
    
    bool cSDL_Window::SetVisibility( const bool _visible ) const
    {
        if( _visible )
            return SDL_ShowWindow( m_window_ );
        return SDL_HideWindow( m_window_ );
    }

} // sk::Platform::
