/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "CameraComponent.h"

#include <sk/Scene/Managers/CameraManager.h>
#include <sk/Scene/Managers/SceneManager.h>
#include <sk/Window/Window.h>

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

	cCameraComponent::cCameraComponent( const cShared_ptr< cSerializedObject >& _object )
	: cComponent( _object->GetBase< iComponent >() )
	{
		_object->BeginRead( this );
		m_type    = _object->ReadData< eType >( "type" ).value_or( eType::kPerspective );
		m_layers_ = _object->ReadData< uint64_t >( "layers" ).value_or( 1 );
		m_camera_settings.auto_resize = _object->ReadData< bool >( "auto_resize" ).value_or( true );
		m_camera_settings.fov         = _object->ReadData< float >( "fov" ).value_or( 70.0f );
		m_camera_settings.near   = _object->ReadData< float >( "near" ).value_or( 0.01f );
		m_camera_settings.far    = _object->ReadData< float >( "far" ).value_or( 2000.0f );
		if( m_camera_settings.auto_resize )
			_autoUpdate();
		else
		{
			m_camera_settings.aspect = _object->ReadData< float >( "aspect" ).value_or( 0.5f );
			const auto viewport_pos          = _object->ReadData< cVector2i32 >( "viewport_pos" ).value_or( cVector2i64{} );
			const auto viewport_size = _object->ReadData< cVector2u64 >( "viewport_size" ).value_or( cVector2u64{} );
			const auto scissor_pos           = _object->ReadData< cVector2i32 >( "scissor_pos" ).value_or( cVector2i64{} );
			const auto scissor_size  = _object->ReadData< cVector2u64 >( "scissor_size" ).value_or( cVector2u64{} );

			m_viewport = {
				.x = viewport_pos.x,
				.y = viewport_pos.y,
				.width  = viewport_size.x,
				.height = viewport_size.y
			};

			m_scissor = {
				.x = scissor_pos.x,
				.y = scissor_pos.y,
				.width  = scissor_size.x,
				.height = scissor_size.y
			};
		}
		_object->EndRead();

		calculateProjectionMatrix();
		Scene::cCameraManager::get().registerCamera( get_shared().Cast< cCameraComponent >() );
	}

	cCameraComponent::~cCameraComponent()
	{
		Scene::cCameraManager::get().UnregisterCamera( get_shared().Cast< cCameraComponent >() );
	}

	void cCameraComponent::renderTo( Graphics::Rendering::cRender_Context& _context )
	{
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
		bool refresh = false;
		if( Platform::GetMainWindow()->WasResizedThisFrame() )
		{
			_autoUpdate();
			refresh = true;
		}

		if( refresh || m_transform_->IsDirty() )
		{
			m_transform_->Update();
			m_transform_->GetWorld().inversed( m_view_proj_inv );
			m_view_proj_inv *= m_projection;
		}
	} // update

	void cCameraComponent::SetSettings( const sCameraSettings& _settings )
	{
		m_camera_settings = _settings;
		calculateProjectionMatrix();
	}

	cShared_ptr< cSerializedObject > cCameraComponent::Serialize()
	{
		auto object = cSerializedObject::CreateForWrite( this );
		object->AddBase( cComponent::Serialize() );
		object->WriteData( "type",          static_cast< uint64_t >( m_type ) );
		object->WriteData( "layers",        m_layers_ );
		object->WriteData( "auto_resize",   m_camera_settings.auto_resize );
		object->WriteData( "fov",           m_camera_settings.fov );
		object->WriteData( "aspect",        m_camera_settings.aspect );
		object->WriteData( "near_clip",     m_camera_settings.near );
		object->WriteData( "far_clip",      m_camera_settings.far );
		object->WriteData( "viewport_pos",  cVector2i64{ m_viewport.x, m_viewport.y } );
		object->WriteData( "viewport_size", cVector2u64{ m_viewport.width, m_viewport.height } );
		object->WriteData( "scissor_pos",   cVector2i64{ m_scissor.x, m_scissor.y } );
		object->WriteData( "scissor_size",  cVector2u64{ m_scissor.width, m_scissor.height } );
		object->EndWrite();
		return object;
	}

	void cCameraComponent::_autoUpdate()
	{
		const auto& main_window  = *Platform::GetMainWindow();
		m_camera_settings.aspect = main_window.GetAspectRatio();
		const auto res = main_window.GetResolution();

		m_viewport = {
			.x = 0,
			.y = 0,
			.width  = res.x,
			.height = res.y
		};

		m_scissor = {
			.x = 0,
			.y = 0,
			.width  = res.x,
			.height = res.y
		};

		calculateProjectionMatrix();
	}

	void cCameraComponent::calculateProjectionMatrix( void )
	{
		m_projection = Math::Matrix4x4::AspectPerspective( m_camera_settings.aspect, m_camera_settings.fov, m_camera_settings.near, m_camera_settings.far );
	} // calculateProjectionMatrix
} // sk::Object::Components::

