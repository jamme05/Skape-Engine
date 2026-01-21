

#include "Forward_Pipeline.h"

#include <Graphics/Passes/Forward_Pass.h>
#include <Platform/Window/Window_Base.h>

sk::Graphics::cForward_Pipeline::cForward_Pipeline( Platform::iWindow* _window )
: cPipeline( _window )
{
    
}

void sk::Graphics::cForward_Pipeline::Initialize()
{
    AddPass< Passes::cForward_Pass >( m_window_ );
    
    cPipeline::Initialize();
}

void sk::Graphics::cForward_Pipeline::Execute()
{
    cPipeline::Execute();
    
    m_window_->SwapBuffers();
}
