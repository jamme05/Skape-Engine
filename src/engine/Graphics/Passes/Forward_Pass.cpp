

#include "Forward_Pass.h"

#include <Graphics/Rendering/Frame_Buffer.h>
#include <Graphics/Rendering/Window_Context.h>
#include <Graphics/Utils/RenderUtils.h>
#include <Platform/Window/Window_Base.h>
#include <Scene/Components/CameraComponent.h>
#include <Scene/Components/MeshComponent.h>
#include <Scene/Managers/Layer_Manager.h>
#include <Scene/Managers/SceneManager.h>

sk::Graphics::Passes::cForward_Pass::cForward_Pass( Platform::iWindow* _window )
: m_window_( _window )
{
    
}

void sk::Graphics::Passes::cForward_Pass::Init()
{
    
}

bool sk::Graphics::Passes::cForward_Pass::Begin()
{
    auto& camera_manager = Scene::cCameraManager::get();
    auto& main_camera    = *camera_manager.getMainCamera();
    
    RenderWithCamera( main_camera );
    
    return true;
}

void sk::Graphics::Passes::cForward_Pass::End()
{
    
}

void sk::Graphics::Passes::cForward_Pass::Destroy()
{
    
}

void sk::Graphics::Passes::cForward_Pass::RenderWithCamera( const Object::Components::cCameraComponent& _camera ) const
{
    const auto& layer_manager = Scene::cLayer_Manager::get();
    
    auto& frame_buffer  = m_window_->GetWindowContext().GetBack();
    
    frame_buffer.Begin( _camera.getViewport(), _camera.getScissor() );
    frame_buffer.Clear( Rendering::eClear::kTargets | Rendering::eClear::kDepth );

    for( auto [ fst, lst ] = layer_manager.GetMeshesIn( _camera.GetLayers() ); fst != lst; ++fst )
    {
        if( !fst.IsValid() )
            continue;
        
        auto& mesh = *fst;
        
        if( !mesh->IsReady() )
            continue;

        const bool res = Utils::RenderMesh( _camera, frame_buffer,
            *mesh->GetMaterial(), mesh->GetTransform().GetWorld(), *mesh->GetMesh() );
        if( !res )
            SK_BREAK;
    }
    
    frame_buffer.End();
}
