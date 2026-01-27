

#include "Deferred_Pipeline.h"

#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Graphics/Passes/GBuffer_Pass.h>
#include <sk/Graphics/Passes/Screen_Pass.h>
#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Platform/Window/Window_Base.h>

#include "sk/Graphics/Passes/Light_Pass.h"

using namespace sk::Graphics;

cDeferred_Pipeline::cDeferred_Pipeline( Platform::iWindow* _window )
: cPipeline( _window )
{
    
}

void cDeferred_Pipeline::Initialize()
{
    m_gbuffer_pass_ = &AddPass< Passes::cGBuffer_Pass >();
    
    auto& asset_manager = cAsset_Manager::get();
    const auto screen_shader   = asset_manager.GetAssetByPath( "shaders/screen.vert" );
    const auto deferred_shader = asset_manager.GetAssetByPath( "shaders/deferred.frag" );
    const auto [ material_meta, _ ] = asset_manager.CreateAsset< Assets::cMaterial >(
        "Deferred Screen Material", Utils::cShader_Link{ screen_shader, deferred_shader }
    );
    m_screen_material_ = material_meta;

    AddPass< Passes::cLight_Pass >();

    AddPass< Passes::cScreen_Pass >( m_window_, material_meta );

    cPipeline::Initialize();
}

void cDeferred_Pipeline::Execute()
{
    cPipeline::Execute();

    const auto& front = m_gbuffer_pass_->GetFront();
    auto& color_buffer = front.GetRenderTarget( 2 );

    m_screen_material_->SetTexture( "Albedo", color_buffer );
}
