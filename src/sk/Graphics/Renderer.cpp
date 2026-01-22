/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Renderer.h"

#include <sk/Graphics/Pipelines/Pipeline.h>
#include <sk/Memory/Tracker/Tracker.h>

// Won't the graphics context have to live within each window?
// Figure out a way to either pick a context depending on the active window,
// or move part of the renderer singleton to the window. Like Window.GetRenderer()


using namespace sk::Graphics;

cRenderer::~cRenderer()
{
    if( m_active_pipeline_ )
        SK_DELETE( m_active_pipeline_ );
}

auto cRenderer::GetPipeline() const -> cPipeline*
{
    return m_active_pipeline_;
}

void cRenderer::SetPipeline( cPipeline* _pipeline )
{
    if( m_active_pipeline_ )
        m_active_pipeline_->Destroy();
    
    m_active_pipeline_ = _pipeline;
    
    if( m_active_pipeline_ )
        m_active_pipeline_->Initialize();
}
