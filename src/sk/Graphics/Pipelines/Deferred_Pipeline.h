

#pragma once

#include <sk/Assets/Material.h>
#include <sk/Graphics/Pipelines/Pipeline.h>

namespace sk::Graphics::Passes
{
    class cGBuffer_Pass;
}

namespace sk::Platform
{
    class iWindow;
} // sk::Platform::

namespace sk::Graphics
{
    class cDeferred_Pipeline : public cPipeline
    {
    public:
        explicit cDeferred_Pipeline( Platform::iWindow* _window );
        void Initialize() override;
        void Execute() override;
        
    private:
        Passes::cGBuffer_Pass*          m_gbuffer_pass_ = nullptr;
        cAsset_Ref< Assets::cMaterial > m_screen_material_;
    };
} // sk::Graphics::
