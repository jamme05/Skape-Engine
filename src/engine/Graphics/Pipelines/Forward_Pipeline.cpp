#include "Forward_Pipeline.h"

#include "Graphics/Passes/Forward_Pass.h"

void sk::Graphics::cForward_Pipeline::Initialize()
{
    AddPass< Passes::cForward_Pass >();
    
    cPipeline::Initialize();
}
