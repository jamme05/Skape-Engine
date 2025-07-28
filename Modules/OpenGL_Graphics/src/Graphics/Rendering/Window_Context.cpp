/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Window_Context.h"

namespace sk::Graphics::Rendering
{
    cWindow_Context::cWindow_Context( const cVector2u32& _resolution, const size_t _frame_buffers )
    : cRender_Context( _frame_buffers )
    , m_resolution_( _resolution )
    {
        
    }

    void cWindow_Context::Resize( const cVector2u32& _new_resolution )
    {
        
    }
} // sk::Graphics::Rendering