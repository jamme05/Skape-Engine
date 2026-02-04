
#pragma once

#include <sk/Math/Vector2.h>

namespace sk::Graphics
{
    namespace Rendering
    {
        class cRender_Context;
    } // sk::Graphics::Rendering::

    class iSurface
    {
    public:
        virtual ~iSurface() = default;

        [[ nodiscard ]]
        virtual auto GetRenderContext() const -> Rendering::cRender_Context& = 0;
        [[ nodiscard ]]
        virtual bool WasResizedThisFrame() const = 0;
        [[ nodiscard ]]
        virtual auto GetResolution() const -> cVector2u32 = 0;

        virtual void SwapBuffers() = 0;
    };
} // sk::Graphics::