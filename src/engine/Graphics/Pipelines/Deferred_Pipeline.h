

#pragma once

#include <Graphics/Pipelines/Pipeline.h>

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
    };
} // sk::Graphics::
