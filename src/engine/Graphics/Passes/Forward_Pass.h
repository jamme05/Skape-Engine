#pragma once

#include <Graphics/Passes/Render_Pass.h>

namespace sk::Platform
{
    class iWindow;
}

namespace sk::Object::Components
{
    class cCameraComponent;
} // sk::Object::Components::

namespace sk::Graphics::Passes
{
    class cForward_Pass : public iPass
    {
    public:
        explicit cForward_Pass( Platform::iWindow* _window );
        
        void Init   () override;
        bool Begin  () override;
        void End    () override;
        void Destroy() override;
        
    private:
        void RenderWithCamera( const Object::Components::cCameraComponent& _camera ) const;
        
        Platform::iWindow* m_window_;
    };
} // sk::Graphics
