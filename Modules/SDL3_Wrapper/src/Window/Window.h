/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Singleton.h>
#include <Platform/Window/Window_Base.h>

#include <SDL3/SDL_init.h>

namespace sk::Platform
{
    class cSDL_Window final : public iWindow
    {
        static cSDL_Window* g_main_window_;
        friend iWindow* GetMainWindow();
    public:
        // TODO: Move to iWindow.
        enum sSettings : uint8_t
        {
            kNone = 0,
            kResizable = SDL_WINDOW_RESIZABLE,
        };
        // TODO: Add fullscreen and default window size options.
        explicit cSDL_Window( const std::string& _name, const cVector2u32& _resolution, uint8_t _flags = kNone );
        ~cSDL_Window() override;

        [[ nodiscard ]]
        bool GetVisibility() const override;
        // Returns true on success.
        [[ nodiscard ]]
        bool SetVisibility( bool _visible ) const override;
        [[ nodiscard ]]
        cVector2u32 GetResolution () const override;
        float       GetAspectRatio() const override;

        void* create_context() override;
        // Void type to skip include I guess
        static SDL_AppResult handle_event( void* _event );

        static SDL_AppResult handle_event( const SDL_KeyboardEvent& _event );
        static SDL_AppResult handle_event( const SDL_GamepadButtonEvent& _event );
        static SDL_AppResult handle_event( const SDL_MouseMotionEvent& _event );
    private:
        bool        m_visible_;
        cVector2u32 m_size_;
        float       m_aspect_ratio_;
        SDL_Window* m_window_;
    };
} // sk::

