#pragma once

#include <Graphics/Passes/Render_Pass.h>

namespace sk::Graphics::Passes
{
    class cForward_Pass : public iPass
    {
    public:
        void Init   () override;
        bool Begin  () override;
        void End    () override;
        void Destroy() override;
    };
} // sk::Graphics
