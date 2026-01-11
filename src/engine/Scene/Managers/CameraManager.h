/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Singleton.h>
#include <Misc/Smart_Ptrs.h>
#include <Scene/Object.h>

namespace sk::Object::Components
{
	class cCameraComponent;
} // sk::Object::Components

#define INVALID_CAMERA_ID ~uint64_t( 0 )

namespace sk::Scene
{
	typedef Object::Components::cCameraComponent camera_t;

	class cCameraManager : public cSingleton< cCameraManager >
	{

	struct sMainCamera
	{
		cShared_ptr< camera_t > m_camera  = nullptr;
		bool                    m_enabled = false;
	};

	public:
		cCameraManager();

		void registerCamera( const cShared_ptr< camera_t >& _camera );

		void setMainCamera( const cShared_ptr< camera_t >& _camera );
		auto getMainCamera(){ return m_main_camera_.m_camera; }

		void setCameraEnabled( const cShared_ptr< camera_t >& _camera, const bool _enable );

		void render();
		
		[[ nodiscard ]]
		auto GetCurrentCamera() const -> const cShared_ptr< camera_t >&;
		void SetCurrentCamera( const cShared_ptr< camera_t >& _camera );

	private:

	// Main camera gets rendered to screen
		sMainCamera m_main_camera_ = {};
		
		cShared_ptr< camera_t > m_current_camera_;
		
		// All registered cameras
		map< uint64_t, cShared_ptr< camera_t > > m_cameras_;
		// All cameras being rendered to ( if they have a set render_context or render_target )
		map< uint64_t, cShared_ptr< camera_t > > m_enabled_cameras_;
	};
} // sk::Scene
