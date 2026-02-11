/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Component.h"

#include <sk/Graphics/Rendering/Render_Context.h>
#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Graphics/Rendering/Scissor.h>
#include <sk/Graphics/Rendering/Viewport.h>
#include <sk/Math/Matrix4x4.h>

namespace sk::Scene
{
	class cCameraManager;
} // sk::Scene

namespace sk::Object::Components
{
	// TODO: Create a generic camera class which will act more like an util.
	SK_COMPONENT_CLASS( CameraComponent )
	{
		SK_CLASS_BODY( CameraComponent )
	public:
		struct sCameraSettings
		{
			bool  auto_resize;
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
		explicit cCameraComponent( const cShared_ptr< cSerializedObject >& _object );
		~cCameraComponent() override;

		void renderTo( Graphics::Rendering::cRender_Context& _context );
		void renderTo( Graphics::Rendering::cRender_Target&  _target );

		void enabled () override;
		void disabled() override;

		void renderAuto(){ if( m_render_target ) renderTo( *m_render_target ); }

		void update() override;

		[[ nodiscard ]] auto& getViewport()     { return m_viewport; }
		[[ nodiscard ]] auto& getScissor ()     { return m_scissor;  }
		[[ nodiscard ]] auto& getViewport() const { return m_viewport; }
		[[ nodiscard ]] auto& getScissor () const { return m_scissor;  }

		[[ nodiscard ]] auto& getViewProjInv() const { return m_view_proj_inv; }
		[[ nodiscard ]] auto& getProjection () const { return m_projection; }
		
		[[ nodiscard ]] auto  GetLayers() const { return m_layers_; }
		
		[[ nodiscard ]] auto& GetSettings() const { return m_camera_settings; }
		void  SetSettings( const sCameraSettings& _settings );

		auto Serialize() -> cShared_ptr< cSerializedObject > override;

	private:

		void _autoUpdate();

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
		
		uint64_t m_layers_ = 1;

		friend class Scene::cCameraManager;
	};
} // sk::Object::Components::

SK_DECLARE_CLASS( sk::Object::Components::CameraComponent )
