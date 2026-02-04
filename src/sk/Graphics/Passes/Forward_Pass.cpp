

#include "Forward_Pass.h"

#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Graphics/Utils/RenderUtils.h>
#include <sk/Platform/Window/Window_Base.h>
#include <sk/Scene/Components/CameraComponent.h>
#include <sk/Scene/Components/MeshComponent.h>
#include <sk/Scene/Managers/Layer_Manager.h>
#include <sk/Scene/Managers/CameraManager.h>

sk::Graphics::Passes::cForward_Pass::cForward_Pass( iSurface* _surface )
: m_surface_( _surface )
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
    
    auto& frame_buffer  = m_surface_->GetRenderContext().GetBack();
    
    frame_buffer.Clear( Rendering::eClear::kTargets | Rendering::eClear::kDepth );
    frame_buffer.Begin( _camera.getViewport(), _camera.getScissor() );

    for( auto [ fst, lst ] = layer_manager.GetMeshesIn( _camera.GetLayers() ); fst != lst; ++fst )
    {
        if( !fst.IsValid() )
            continue;
        
        auto& mesh = *fst;
        
        if( !mesh->IsReady() )
            continue;

        const bool res = Utils::RenderMesh( _camera.getViewProjInv(), frame_buffer,
            *mesh->GetMaterial(), mesh->GetTransform().GetWorld(), *mesh->GetMesh() );
        if( !res )
            SK_BREAK;
    }
    
    frame_buffer.End();
}
