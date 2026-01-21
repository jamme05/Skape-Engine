

#include "GBuffer_Pass.h"

#include <App.h>
#include <Debugging/Macros/Assert.h>
#include <Graphics/Pipelines/Pipeline.h>
#include <Graphics/Rendering/Depth_Target.h>
#include <Graphics/Rendering/Frame_Buffer.h>
#include <Graphics/Utils/RenderUtils.h>
#include <Platform/Window/Window_Base.h>
#include <Scene/Components/CameraComponent.h>
#include <Scene/Components/MeshComponent.h>
#include <Scene/Managers/CameraManager.h>
#include <Scene/Managers/Layer_Manager.h>

using namespace sk::Graphics::Passes;

void cGBuffer_Pass::Init()
{
    m_render_context_ = std::make_unique< Rendering::cRender_Context >( 2, 3 );
    
    auto resolution = getPipeline().GetWindow()->GetResolution();
    
    for( const auto& frame_buffer : *m_render_context_ )
    {
        auto color_target    = sk::make_shared< Rendering::cRender_Target >( resolution, Rendering::cRender_Target::eFormat::kRGBA8 );
        auto position_target = sk::make_shared< Rendering::cRender_Target >( resolution, Rendering::cRender_Target::eFormat::kRGBA16F );
        auto normal_target   = sk::make_shared< Rendering::cRender_Target >( resolution, Rendering::cRender_Target::eFormat::kRGBA16F );
        
        auto depth_target = sk::make_shared< Rendering::cDepth_Target >( resolution, Rendering::cDepth_Target::eFormat::kD24FS8 );
        
        frame_buffer->Bind( 0, position_target );
        frame_buffer->Bind( 1, normal_target );
        frame_buffer->Bind( 2, color_target );
        
        frame_buffer->Bind( depth_target );
    }
}

bool cGBuffer_Pass::Begin()
{
    auto  main_window    = cApp::get().GetMainWindow();
    auto& camera_manager = Scene::cCameraManager::get();
    auto& main_camera    = *camera_manager.getMainCamera();
    
    if( main_window->WasResizedThisFrame() )
        m_render_context_->Resize( main_window->GetResolution() );
    
    RenderWithCamera( main_camera );
    
    return true;
}

void cGBuffer_Pass::End()
{
    m_render_context_->End();
}

void cGBuffer_Pass::Destroy()
{
    m_render_context_.reset();
}

void cGBuffer_Pass::RenderWithCamera( const Object::Components::cCameraComponent& _camera ) const
{
    const auto& layer_manager = Scene::cLayer_Manager::get();
    
    auto& frame_buffer = m_render_context_->GetBack();
    
    frame_buffer.Begin( _camera.getViewport(), _camera.getScissor() );
    frame_buffer.Clear( Rendering::eClear::kAll );
    
    for( auto [ fst, lst ] = layer_manager.GetMeshesIn( _camera.GetLayers() ); fst != lst; ++fst )
    {
        if( !fst.IsValid() )
            continue;
        
        auto& mesh = *fst;
        
        if( !mesh->IsReady() )
            continue;

        // TODO: Add a local render function that's more optimized
        const bool res = Utils::RenderMesh( _camera, frame_buffer,
            *mesh->GetMaterial(), mesh->GetTransform().GetWorld(), *mesh->GetMesh() );
        if( !res )
            SK_BREAK;
    }
}
