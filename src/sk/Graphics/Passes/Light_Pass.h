

#pragma once

#include "Render_Pass.h"

#include <sk/Assets/Material.h>
#include <sk/Assets/Access/Asset_Ref.h>

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
        cAsset_Ref< Assets::cMaterial > m_shadow_material_;
    };
} // sk::Graphics::Passes::
