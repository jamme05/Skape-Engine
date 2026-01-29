/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "CameraComponent.h"

#include <sk/Scene/Managers/CameraManager.h>
#include <sk/Scene/Managers/SceneManager.h>

namespace sk::Object::Components
{
	cCameraComponent::cCameraComponent( const Graphics::sViewport& _viewport, const Graphics::sScissor& _scissors, const sCameraSettings& _settings, const eType _type )
	: m_camera_settings( _settings )
	, m_viewport( _viewport )
	, m_scissor( _scissors )
	, m_type( _type )
	{
		calculateProjectionMatrix();
		Scene::cCameraManager::get().registerCamera( get_shared().Cast< cCameraComponent >() );
	} // cCamera

	void cCameraComponent::renderTo( Graphics::Rendering::cRender_Context& _context )
	{
		cSceneManager::get().render_with( *this, _context );
	} // begin

	void cCameraComponent::renderTo( Graphics::Rendering::cRender_Target& _context )
	{
		// TODO: Default pipeline for single render targets.
		// renderTo( default_render_target_context )
	} // renderTo

	void cCameraComponent::enabled( void )
	{
		Scene::cCameraManager::get().setCameraEnabled( get_shared().Cast< cCameraComponent >(), true );
	} // enabled

	void cCameraComponent::disabled( void )
	{
		Scene::cCameraManager::get().setCameraEnabled( get_shared().Cast< cCameraComponent >(), false );
	} // disabled
	void cCameraComponent::update( void )
	{
		if( m_transform_->IsDirty() )
		{
			m_transform_->Update();
			m_transform_->GetWorld().inversed( m_view_proj_inv );
			m_view_proj_inv *= m_projection;
		}
	} // update

	void cCameraComponent::calculateProjectionMatrix( void )
	{
		m_projection = Math::Matrix4x4::AspectPerspective( m_camera_settings.aspect, m_camera_settings.fov, m_camera_settings.near, m_camera_settings.far );
	} // calculateProjectionMatrix
} // sk::Object::Components::

