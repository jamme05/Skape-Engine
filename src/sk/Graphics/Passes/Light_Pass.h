

#pragma once

#include "Render_Pass.h"

#include <sk/Assets/Material.h>
#include <sk/Assets/Access/Asset_Ref.h>
#include <sk/Graphics/Rendering/Scissor.h>
#include <sk/Graphics/Rendering/Viewport.h>

namespace sk::Graphics::Rendering
{
    class cRender_Context;
}

namespace sk::Object::Components
{
    class cLightComponent;
} // sk::Object::Components

namespace sk::Graphics::Passes
{
    class cLight_Pass : public iPass
    {
    public:
        void Init   () override;
        bool Begin  () override;
        void End    () override;
        void Destroy() override;
        
    private:
        void _shadowPass( const Object::Components::cLightComponent& _light );

        static auto _getViewport( const Object::Components::cLightComponent& _light ) -> sViewport;
        static auto _getScissor( const Object::Components::cLightComponent& _light ) -> sScissor;

        cVector2u32                     m_atlas_size       = { 1024, 1024 };
        Rendering::cRender_Context*     m_shadow_context_  = nullptr;
        cAsset_Ref< Assets::cMaterial > m_shadow_material_;
    };
} // sk::Graphics::Passes::
