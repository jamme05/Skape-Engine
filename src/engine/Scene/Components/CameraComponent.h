/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Component.h"

#include <Graphics/Rendering/Scissor.h>
#include <Graphics/Rendering/Viewport.h>
#include <Graphics/Rendering/Render_Context.h>
#include <Graphics/Rendering/Render_Target.h>

#include <Math/Matrix4x4.h>

namespace sk::Scene
{
	class cCameraManager;
} // sk::Scene

namespace sk::Object::Components
{
	SK_COMPONENT_CLASS( CameraComponent )
	{
		SK_CLASS_BODY( CameraComponent )
	public:
		struct sCameraSettings
		{
			float fov;
			float aspect;
			float near;
			float far;
		};

		enum class eType : uint8_t
		{
			kPerspective,
			kOrthographic,
		};

		cCameraComponent( const Graphics::sViewport& _viewport, const Graphics::sScissor& _scissors, const sCameraSettings& _settings, eType _type );

		void renderTo( Graphics::Rendering::cRender_Context& _context );
		void renderTo( Graphics::Rendering::cRender_Target&  _target );

		void enabled ( void ) override;
		void disabled( void ) override;

		void renderAuto( void ){ if( m_render_target ) renderTo( *m_render_target ); }

		void update( void ) override;

		auto& getViewport( void )       { return m_viewport; }
		auto& getViewport( void ) const { return m_viewport; }
		auto& getScissor ( void )       { return m_scissor;  }
		auto& getScissor ( void ) const { return m_scissor;  }

		auto& getViewProjInv( void ) const { return m_view_proj_inv; }
		auto& getProjection ( void ) const { return m_projection; }

	private:

		void calculateProjectionMatrix( void );

		sCameraSettings m_camera_settings;

		// TODO: Add support for render target, custom render pipeline in that case.
		Graphics::Rendering::cRender_Target* m_render_target;

		Graphics::sViewport m_viewport;
		Graphics::sScissor  m_scissor;

		// TODO: Rename to view_proj as view is the inverted world.
		cMatrix4x4f m_view_proj_inv;
		cMatrix4x4f m_projection;

		eType m_type;

		constexpr static auto invalid_camera_id = ~0llu;

		uint64_t m_camera_id = invalid_camera_id;

		friend class Scene::cCameraManager;
	};
} // sk::Object::Components::

DECLARE_CLASS( sk::Object::Components::CameraComponent )
