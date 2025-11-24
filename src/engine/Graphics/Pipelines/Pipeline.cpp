#include "Pipeline.h"

#include <Debugging/Debugging.h>
#include <Graphics/Passes/Render_Pass.h>

#include "Graphics/Renderer_Impl.h"
#include "Scene/Managers/EventManager.h"
#include "Scene/Managers/SceneManager.h"

void sk::Graphics::cPipeline::Initialize()
{
    for( const auto& pass : m_passes_ )
    {
        pass->m_current_pipeline_ = this;
        pass->Init();
    }
    
    m_initialized_ = true;
} // Initialize

void sk::Graphics::cPipeline::Begin()
{
    for( const auto& pass : m_passes_ )
    {
        if( pass->Begin() )
        {
            // TODO: Figure out the logic to have here
            pass->End();
        }
    }
} // Start

void sk::Graphics::cPipeline::End()
{
    
} // End

void sk::Graphics::cPipeline::Destroy()
{
    
} // Destroy

void sk::Graphics::cPipeline::AddPass( std::unique_ptr< Passes::iPass >&& _pass )
{
    SK_ERR_IF( _pass == nullptr, "Render Pass is null" )
    SK_BREAK_RET_IF( sk::Severity::kGraphics, m_initialized_,
        "Unable to add a new pass once initialized." )
    
    m_passes_.push_back( std::move( _pass ) );
} // AddPass

auto sk::Graphics::cPipeline::GetPass( const size_t _index ) const -> Passes::iPass&
{
    SK_ERR_IF( _index >= m_passes_.size(), "Pass out of bounds" )
    return *m_passes_[ _index ];
}
