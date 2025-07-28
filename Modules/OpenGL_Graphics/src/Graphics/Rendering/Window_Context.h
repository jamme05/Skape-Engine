/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Rendering/Render_Context.h>

namespace sk::Graphics::Rendering
{
    class cWindow_Context : public cRender_Context
    {
    public:
        cWindow_Context( const cVector2u32& _resolution, size_t _frame_buffers = 2 );

        void Resize( const cVector2u32& _new_resolution );

    private:
        cVector2u32  m_resolution_;
    };
} // sk::Graphics::Rendering
