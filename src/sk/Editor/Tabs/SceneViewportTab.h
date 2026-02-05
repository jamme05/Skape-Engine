
#pragma once

#include <sk/Editor/Tabs/Tab.h>
#include <sk/Graphics/Utils/RenderSurface.h>
#include <sk/Misc/Smart_Ptrs.h>

namespace sk::Graphics
{
    class cPipeline;
}

namespace sk::Object
{
    class cCameraFlight;
}

namespace sk::Editor::Tabs
{
    class cSceneViewportTab : public aTab
    {
    public:
        explicit cSceneViewportTab( const std::string& _name ) : aTab( _name ){}

        void Create () override;
        void Draw   () override;
        void Destroy() override;

    private:
        Graphics::cPipeline* m_pipeline_ = nullptr;
        cShared_ptr< Object::cCameraFlight > m_camera_ = nullptr;
        std::unique_ptr< Graphics::Utils::cRenderSurface > m_surface_;
        cVector2u32 m_resolution_ = {};
    };
} // sk::Editor::Tabs::

