#include "Forward_Pass.h"

#include <Scene/Managers/SceneManager.h>

void sk::Graphics::Passes::cForward_Pass::Init()
{
    
}

bool sk::Graphics::Passes::cForward_Pass::Begin()
{
    // TODO: Add some way to filter by layer.
    cSceneManager::render();
    return true;
}

void sk::Graphics::Passes::cForward_Pass::End()
{
    
}

void sk::Graphics::Passes::cForward_Pass::Destroy()
{
    
}
