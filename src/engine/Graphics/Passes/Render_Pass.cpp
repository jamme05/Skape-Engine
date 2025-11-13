#include "Render_Pass.h"

auto sk::Graphics::Passes::iPass::getPipeline() const -> cPipeline&
{
    return *m_current_pipeline_;
} // getPipeline