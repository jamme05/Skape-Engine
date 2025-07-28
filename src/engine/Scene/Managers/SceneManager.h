/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "CameraManager.h"

#include "Math/Matrix4x4.h"
#include "Misc/Singleton.h"
#include "Misc/Smart_Ptrs.h"

namespace sk
{
	namespace Graphics::Rendering
	{
		class cRender_Context;
	}

	class cScene;

	class cSceneManager : public cSingleton< cSceneManager >
	{

	public:
		struct sObjectBuffer
		{
			cMatrix4x4f view_proj_inv;
			cMatrix4x4f world;
		};
		// TODO: Logic?
		 cSceneManager( void );
		~cSceneManager( void );

		void registerScene( const cShared_ptr< cScene >& _scene );

		void update( void );

		static void render( void );

		void render_with( sk::Object::Components::cCameraComponent& _camera, Graphics::Rendering::cRender_Context& _context, const bool _swap = true );

		static auto get_active_context( void ){ return m_active_context; }

		static auto& get_buffer( void ){ return m_out_buffer; }

	private:

		static sObjectBuffer* m_out_buffer;

		static Graphics::Rendering::cRender_Context* m_active_context;

		vector< cShared_ptr< cScene > > m_scenes;
	};
} // sk::
