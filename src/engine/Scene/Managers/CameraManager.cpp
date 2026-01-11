/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "CameraManager.h"

#include <Graphics/Renderer_Impl.h>
#include <Scene/Components/CameraComponent.h>
#include <Scene/Managers/SceneManager.h>

namespace sk::Scene
{
	cCameraManager::cCameraManager( void )
	{
	} // cCameraManager

	void cCameraManager::registerCamera( const cShared_ptr< camera_t >& _camera )
	{
		_camera->m_camera_id = m_cameras_.size();

		m_cameras_[ _camera->m_camera_id ] = _camera;
	} // registerCamera

	void cCameraManager::setMainCamera( const cShared_ptr< camera_t >& _camera )
	{
		// Add previous main camera if it was enabled.
		if( m_main_camera_.m_enabled )
			m_cameras_[ m_main_camera_.m_camera->m_camera_id ] = m_main_camera_.m_camera;

		if( _camera->m_camera_id == INVALID_CAMERA_ID )
		{
			SK_WARNING( sk::Severity::kGeneral | 2000,
				"WARNING: Camera isn't registered. You won't be stopped for doing this. But it isn't recommended." )

			m_main_camera_.m_enabled = false;
		}
		else
		{
			// If an element was erased it means the camera was enabled.
			m_main_camera_.m_enabled = ( m_enabled_cameras_.erase( _camera->m_camera_id ) == 1 );
		}

		m_main_camera_.m_camera = _camera;
	} // setMainCamera

	void cCameraManager::setCameraEnabled( const cShared_ptr< camera_t >& _camera, const bool _enable )
	{
		SK_WARN_IF_RET( sk::Severity::kGeneral | 2000, _camera->m_camera_id == INVALID_CAMERA_ID,
			"WARNING: Camera isn't registered. Register it before you add it to the list." )

		if( _enable )
			m_enabled_cameras_[ _camera->m_camera_id ] = _camera;
		else
			m_enabled_cameras_.erase( _camera->m_camera_id );

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
