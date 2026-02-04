

#include "Forward_Pipeline.h"

#include <sk/Graphics/Passes/Forward_Pass.h>
#include <sk/Platform/Window/Window_Base.h>

sk::Graphics::cForward_Pipeline::cForward_Pipeline( Platform::iWindow* _window )
: cPipeline( _window )
{
    
}

void sk::Graphics::cForward_Pipeline::Initialize()
{
    AddPass< Passes::cForward_Pass >( m_surface_ );
    
    cPipeline::Initialize();
}

void sk::Graphics::cForward_Pipeline::Execute()
{
    cPipeline::Execute();
    
    m_surface_->SwapBuffers();
}
