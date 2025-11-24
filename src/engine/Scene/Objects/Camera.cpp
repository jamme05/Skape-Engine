/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Camera.h"
#include "Scene/Components/CameraComponent.h"

#include <Graphics/Renderer_Impl.h>
#include <Window/Window.h>
#include <Scene/Managers/CameraManager.h> 

#include "App.h"

namespace sk::Object
{
	cCamera::cCamera( const std::string& _name )
	: cObject( _name )
	{
		const auto& app = cApp::get();
		const auto  main_window = app.GetMainWindow();
		const auto  resolution   = main_window->GetResolution();
		const auto  aspect       = main_window->GetAspectRatio();

		const Graphics::sViewport viewport = { .x = 0, .y = 0, .width = resolution.x, .height = resolution.y };
		const Graphics::sScissor  scissor  = { .x = 0, .y = 0, .width = resolution.x, .height = resolution.y };

		Components::cCameraComponent::sCameraSettings settings{ 70.0f, aspect, 0.1f, 2000.0f };

		m_camera = AddComponent< Components::cCameraComponent >( viewport, scissor, settings, Components::cCameraComponent::eType::kPerspective );
		m_root   = m_camera;
		// Update root as old root has been deleted.
		m_camera->SetParent( nullptr );
	}

	void cCamera::setAsMain( void ) const
	{
		Scene::cCameraManager::get().setMainCamera( m_camera );
	} // setAsMain
} // sk::Object::
