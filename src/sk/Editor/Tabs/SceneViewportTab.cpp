
#include "SceneViewportTab.h"

#include <sk/Graphics/Renderer.h>
#include <sk/Graphics/Pipelines/Deferred_Pipeline.h>
#include <sk/Graphics/Pipelines/Pipeline.h>
#include <sk/Graphics/Rendering/Depth_Target.h>
#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Scene/Components/CameraComponent.h>
#include <sk/Scene/Objects/CameraFlight.h>

#include <imgui.h>

using namespace sk::Editor::Tabs;


void cSceneViewportTab::Create()
{
	m_surface_ = std::make_unique< Graphics::Utils::cRenderSurface >( cVector2u32{ 1280, 720 } );
    m_camera_  = sk::MakeShared< Object::cCameraFlight >( "Editor Camera" );
    m_camera_->setAsMain();
    m_camera_->setFilter( m_camera_->getFilter() | Input::kEditor );

	m_pipeline_ = SK_SINGLE( sk::Graphics::cDeferred_Pipeline, m_surface_.get() );
    m_pipeline_->Initialize();
}

void cSceneViewportTab::Draw()
{
    const auto region = ImGui::GetContentRegionAvail();
    if( region.x <= 1.0f || region.y <= 1.0f )
        return;

    m_surface_->SetResolution( cVector2u32( region.x, region.y ) );

    auto resolution  = m_surface_->GetResolution();
    if( auto& back = m_surface_->GetRenderContext().GetBack(); resolution != back.GetDepthTarget()->GetResolution() )
        back.Resize( resolution );

    m_camera_->update();
    auto camera = m_camera_->GetComponent< Object::Components::cCameraComponent >();
    auto settings = camera->GetSettings();
    settings.aspect = static_cast< float >( resolution.x ) / static_cast< float >( resolution.y );
    camera->SetSettings( settings );
    camera->getViewport().width  = resolution.x;
    camera->getViewport().height = resolution.y;
    camera->getScissor ().width  = resolution.x;
    camera->getScissor ().height = resolution.y;

    m_pipeline_->Execute();

    const auto& front = m_surface_->GetRenderContext().GetFront();

    ImGui::Image( front.GetRenderTarget( 0 )->get_native_texture(), region, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
}

void cSceneViewportTab::Destroy()
{
    m_camera_ = nullptr;
    SK_DELETE( m_pipeline_ );
    m_surface_.reset();
}
