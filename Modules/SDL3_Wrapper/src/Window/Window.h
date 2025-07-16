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
    public:
        // TODO: Move to iWindow.
        enum sSettings : uint8_t
        {
            kNone = 0,
            kResizable = SDL_WINDOW_RESIZABLE,
        };
        explicit cSDL_Window( const std::string& _name, const cVector2i& _size, uint8_t _flags = kNone );
        ~cSDL_Window() override;

        bool SetVisibility( bool _visible ) const override;

        void* create_context() override;
        // Void type to skip include I guess
        static SDL_AppResult handle_event( void* _event );

        static SDL_AppResult handle_event( const SDL_KeyboardEvent& _event );
    private:
        SDL_Window* m_window_;
    };
} // sk::

