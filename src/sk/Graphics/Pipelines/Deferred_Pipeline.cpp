

#include "Deferred_Pipeline.h"

#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Graphics/Passes/GBuffer_Pass.h>
#include <sk/Graphics/Passes/Light_Pass.h>
#include <sk/Graphics/Passes/Screen_Pass.h>
#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Graphics/Rendering/Window_Context.h>
#include <sk/Platform/Window/Window_Base.h>

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
    const auto [ material_meta, material ] = asset_manager.CreateAsset< Assets::cMaterial >(
        "Deferred Screen Material", Utils::cShader_Link{ screen_shader, deferred_shader }
    );
    m_screen_material_ = material_meta;
    material->SetDepthTest( Assets::cMaterial::eDepthTest::kDisabled );

    m_light_pass = &AddPass< Passes::cLight_Pass >();

    if( m_window_ )
    {
        auto& context = m_window_->GetWindowContext();
        AddPass< Passes::cScreen_Pass >( context, material_meta );
    }
    else
    {
        auto resolution = Platform::GetMainWindow()->GetResolution();
        m_fallback_window_context_ = std::make_unique< Rendering::cRender_Context >();
        for( const auto& frame_buffer : *m_fallback_window_context_ )
            frame_buffer->Bind( sk::make_shared< Rendering::cRender_Target >( resolution, Rendering::cRender_Target::eFormat::kRGBA8 ) );

        AddPass< Passes::cScreen_Pass >( *m_fallback_window_context_, material_meta );
    }

    cPipeline::Initialize();
}

void cDeferred_Pipeline::Execute()
{
    cPipeline::Execute();

    const auto& front = m_gbuffer_pass_->GetFront();
    auto& color_buffer = front.GetRenderTarget( 2 );

    m_screen_material_->SetTexture( "Albedo", color_buffer );

    if( m_fallback_window_context_ != nullptr )
        m_fallback_window_context_->End();
}
