

#include "Deferred_Pipeline.h"

#include "Assets/Management/Asset_Manager.h"
#include "Graphics/Passes/GBuffer_Pass.h"
#include "Graphics/Passes/Screen_Pass.h"

using namespace sk::Graphics;

cDeferred_Pipeline::cDeferred_Pipeline( Platform::iWindow* _window )
: cPipeline( _window )
{
    
}

void cDeferred_Pipeline::Initialize()
{
    AddPass< Passes::cGBuffer_Pass >();
    
    auto& asset_manager = cAsset_Manager::get();
    const auto screen_shader   = asset_manager.GetAssetByPath( "shaders/screen.vert" );
    const auto deferred_shader = asset_manager.GetAssetByPath( "shaders/deferred.frag" );
    auto material = asset_manager.CreateAsset< Assets::cMaterial >(
        "Deferred Screen Material", Utils::cShader_Link{ screen_shader, deferred_shader }
    );
    
    AddPass< Passes::cScreen_Pass >( m_window_, material );
    
    cPipeline::Initialize();
}

void cDeferred_Pipeline::Execute()
{
    cPipeline::Execute();
}
