/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Component.h"
#include "Assets/Asset_Ptr.h"
#include "Assets/Mesh.h"

namespace sk::Assets {
	class cTexture;
}

namespace sk::Scene
{
	class cCameraManager;
} // sk::Scene

namespace sk::Object::Components
{
	SK_COMPONENT_CLASS( MeshComponent )
	{
		SK_CLASS_BODY( MeshComponent )
	public:
		explicit cMeshComponent( const cShared_ptr< cAsset_Meta >& _mesh );

		void render() override;
		void enabled() override;
		void disabled() override;

		void SetMesh( const cShared_ptr< cAsset_Meta >& _mesh );
		void SetTexture( const cShared_ptr< cAsset_Meta >& _texture );

	private:
		cAsset_Ptr m_mesh_;
		cAsset_Ptr m_texture_;
	};
} // sk::Object::Components

DECLARE_CLASS( sk::Object::Components::MeshComponent )