/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "CameraManager.h"

#include <sk/Graphics/Renderer_Impl.h>
#include <sk/Scene/Components/CameraComponent.h>
#include <sk/Scene/Managers/SceneManager.h>

namespace sk::Scene
{
	cCameraManager::cCameraManager( void )
	{
	} // cCameraManager

	void cCameraManager::registerCamera( const cShared_ptr< camera_t >& _camera )
	{
		m_cameras_[ _camera->GetUUID() ] = _camera;
	} // registerCamera

	void cCameraManager::UnregisterCamera( const cShared_ptr< camera_t >& _camera )
	{
		if( m_main_camera_.m_camera == _camera )
			m_main_camera_ = {};
		m_cameras_.erase( _camera->GetUUID() );
	}

	void cCameraManager::setMainCamera( const cShared_ptr< camera_t >& _camera )
	{
		// Add previous main camera if it was enabled.
		if( m_main_camera_.m_enabled )
			m_cameras_[ m_main_camera_.m_camera->GetUUID() ] = m_main_camera_.m_camera;

		m_main_camera_.m_camera = _camera;
	} // setMainCamera

	void cCameraManager::setCameraEnabled( const cShared_ptr< camera_t >& _camera, const bool _enable )
	{
		if( _enable )
			m_enabled_cameras_[ _camera->GetUUID() ] = _camera;
		else
			m_enabled_cameras_.erase( _camera->GetUUID() );

	} // setCameraEnabled

	void cCameraManager::render( void )
	{
		// Deprecated
		for( auto& camera : m_enabled_cameras_ | std::views::values )
			camera->renderAuto();
	} // render
	
	auto cCameraManager::GetCurrentCamera() const -> const cShared_ptr< camera_t >&
	{
		return m_current_camera_;
	}

	void cCameraManager::SetCurrentCamera( const cShared_ptr< camera_t >& _camera )
	{
		m_current_camera_ = _camera;
	}
} // sk::Scene::
