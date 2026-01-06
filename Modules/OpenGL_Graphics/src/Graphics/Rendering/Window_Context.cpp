/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Window_Context.h"

#include "Platform/Window/Window_Base.h"

namespace sk::Graphics::Rendering
{
    cWindow_Context::cWindow_Context( Platform::iWindow& _window )
    : cRender_Context( 1 )
    , m_window_( &_window )
    , m_resolution_( _window.GetResolution() )
    {
        
    }

    void cWindow_Context::Resize( const cVector2u32& _new_resolution )
    {
        
    }

    void cWindow_Context::Swap()
    {
        cRender_Context::Swap();
    }
} // sk::Graphics::Rendering