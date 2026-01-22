/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Graphics/Rendering/Render_Context.h>

namespace sk::Platform
{
    class iWindow;
}

namespace sk::Graphics::Rendering
{
    class cWindow_Context : public cRender_Context
    {
    public:
        cWindow_Context( Platform::iWindow& _window );

        void Resize( const cVector2u32& _new_resolution );
        
        void Swap() override;

    private:
        
        
        Platform::iWindow* m_window_;
        cVector2u32 m_resolution_;
    };
} // sk::Graphics::Rendering
