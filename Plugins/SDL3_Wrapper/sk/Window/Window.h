/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Misc/Singleton.h>
#include <sk/Misc/Smart_Ptrs.h>
#include <sk/Platform/Window/Window_Base.h>

#include <SDL3/SDL_init.h>

#include <vector>

namespace sk::Graphics::Rendering
{
    class cWindow_Context;
}

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
        explicit cSDL_Window( const std::string_view& _name, const cVector2u32& _resolution, uint8_t _flags = kNone );
        ~cSDL_Window() override;

        void Init() override;
        [[ nodiscard ]]
        bool GetVisibility() const override;
        // Returns true on success.
        [[ nodiscard ]]
        bool SetVisibility( bool _visible ) const override;
        [[ nodiscard ]]
        cVector2u32 GetResolution () const override;
        [[ nodiscard ]]
        float       GetAspectRatio() const override;
        void        SetMouseCapture( bool _capture ) override;

        [[ nodiscard ]]
        auto GetRenderContext() const -> Graphics::Rendering::cRender_Context& override;
        void SwapBuffers() override;

        void UseContext() override;

        [[ nodiscard ]]
        bool WasResizedThisFrame() const override;

        // For internal usage.
        [[ nodiscard ]]
        auto  get_window  () const { return m_window_; }
        [[ nodiscard ]]
        auto& get_context() const { return m_context_; }

        // TODO: Maybe move these outside of the window class?
        // Void type to skip include I guess
        static SDL_AppResult handle_event( void* _event );

        static SDL_AppResult handle_event( const SDL_WindowEvent& _event );
        static SDL_AppResult handle_event( const SDL_KeyboardEvent& _event );
        static SDL_AppResult handle_event( const SDL_GamepadButtonEvent& _event );
        static SDL_AppResult handle_event( const SDL_MouseMotionEvent& _event );
        static SDL_AppResult handle_event( const SDL_MouseButtonEvent& _event );
    private:
        void add_self();
        void remove_self() const;
        void resize( cVector2u32 _new_resolution );
        
        bool        m_visible_;
        cVector2u32 m_size_;
        float       m_aspect_ratio_;
        SDL_Window* m_window_;
        uint64_t    m_resized_on_frame_ = 0;
        
        std::unique_ptr< Graphics::Rendering::cWindow_Context > m_window_context_;
        
        SDL_GLContext m_context_;
    };
} // sk::

