// Window functions:

#ifdef SK_GRAPHICS_OPENGL

#include "Window.h"

#include <sk/Debugging/Debugging.h>
#include <sk/Graphics/Rendering/Window_Context.h>
#include <sk/Memory/Tracker/Tracker.h>
#include <sk/Platform/Platform_Base.h>
#include <sk/Platform/Time.h>
#include <sk/Platform/ImGui/ImGuiHelper.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

#include <glbinding/glbinding.h>



using namespace sk::Platform;

cSDL_Window::cSDL_Window( const std::string_view& _name, const cVector2u32& _resolution, const uint8_t _flags )
: m_visible_( false )
, m_size_( _resolution )
, m_aspect_ratio_( Vector2::Aspect( _resolution ) )
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, SK_OPENGL_MAJOR_VERSION );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, SK_OPENGL_MINOR_VERSION );

    const SDL_WindowFlags flags = _flags | SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE;
    m_window_ = SDL_CreateWindow( _name.data(), static_cast< int >( _resolution.x ), static_cast< int >( _resolution.y ), flags );
    SK_ERR_IF( m_window_ == nullptr,
        TEXT( "ERROR: Unable to create a window.\n Reason: {}", SDL_GetError() ) )

    if( g_main_window_ == nullptr )
        g_main_window_ = this;

    m_context_ = SDL_GL_CreateContext( m_window_ );
    SDL_GL_MakeCurrent( m_window_, m_context_ );
    
    add_self();
} // cSDL_Window


cSDL_Window::~cSDL_Window()
{
    remove_self();

    if( g_main_window_ == this )
        g_main_window_ = nullptr;
    
    SDL_GL_DestroyContext( m_context_ );
    
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

void cSDL_Window::UseContext()
{
    SDL_GL_MakeCurrent( m_window_, m_context_ );
}

bool cSDL_Window::WasResizedThisFrame() const
{
    return m_resized_on_frame_ == Time::Frame;
}

function_ptr_t sk::Platform::get_proc_address( const char* _name )
{
    return SDL_GL_GetProcAddress( _name );
} // getProcAddress

void sk::Gui::InitImGui( const iWindow* _target_window )
{
    const auto window = static_cast< const cSDL_Window* >( _target_window );
    ImGui_ImplSDL3_InitForOpenGL( window->get_window(), window->get_context() );
    ImGui_ImplOpenGL3_Init();
}

void sk::Gui::ImGuiUpdateWindows()
{
    if( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void sk::Gui::ImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void sk::Gui::ImGuiRender()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

void sk::Gui::ImGuiShutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
}

#endif // SK_GRAPHICS_OPENGL

