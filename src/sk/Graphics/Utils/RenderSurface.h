
#pragma once

#include <sk/Graphics/Surface.h>
#include <sk/Graphics/Rendering/Render_Context.h>

#include <memory>

namespace sk::Graphics::Utils
{
    class cRenderSurface : public iSurface
    {
    public:
        explicit cRenderSurface( cVector2u32 _resolution );

        [[ nodiscard ]]
        bool WasResizedThisFrame() const override;
        [[ nodiscard ]]
        auto GetRenderContext() const -> Rendering::cRender_Context& override;
        [[ nodiscard ]]
        auto GetResolution() const -> cVector2u32 override;
        void SetResolution( cVector2u32 _resolution, bool _resize_context = false );

        void SwapBuffers() override;
    private:
        std::unique_ptr< Rendering::cRender_Context > m_render_context_;
        cVector2u32 m_resolution_;
        uint64_t    m_resized_on_frame_ = 0;
    };
} // sk::Graphics::Utils
