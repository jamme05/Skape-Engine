/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Graphics/Pipelines/Pipeline.h>

namespace sk::Platform
{
    class iWindow;
}

namespace sk::Graphics
{
    class cForward_Pipeline final : public cPipeline
    {
    public:
        explicit cForward_Pipeline( Platform::iWindow* _window );
        void Initialize() override;
        void Execute() override;
    };
} // sk::Graphics
