

#pragma once

#include <memory>

#include <Graphics/Passes/Render_Pass.h>
#include <Graphics/Rendering/Render_Context.h>

namespace sk::Object::Components
{
    class cCameraComponent;
} // sk::Object::Components::

namespace sk::Graphics::Passes
{
    class cGBuffer_Pass : public iPass
    {
    public:
        void Init   () override;
        bool Begin  () override;
        void End    () override;
        void Destroy() override;
        
        void RenderWithCamera( const Object::Components::cCameraComponent& _camera ) const;

        auto& GetFront() const { return m_render_context_->GetFront(); }
        
    private:
        std::unique_ptr< Rendering::cRender_Context > m_render_context_;
    };
} // sk::Graphics::Passes::
