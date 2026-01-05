 /*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "MeshComponent.h"

#include "CameraComponent.h"
#include "Assets/Texture.h"
#include "Scene/Managers/SceneManager.h"

namespace sk::Object::Components
{
	cMeshComponent::cMeshComponent( const cShared_ptr< cAsset_Meta >& _mesh, const cShared_ptr< cAsset_Meta >& _material )
	: m_mesh_( get_weak(), _mesh )
	, m_material_( get_weak(), _material )
	{
		m_mesh_.on_asset_loaded += []( const cAsset& _asset )
		{
			sk::println( "Asset with name: {} was loaded.", _asset.GetMeta()->GetName().view() );
		};
	}

	void cMeshComponent::render()
	{
		if( m_mesh_.IsLoaded() )
			;
			// DoSomeInternalRender();
	}

	void cMeshComponent::enabled()
	{
		if( m_mesh_.IsValid() )
			m_mesh_.LoadAsync();
		if( m_material_.IsValid() )
			m_material_.LoadAsync();
	}

	void cMeshComponent::disabled()
	{
		if( m_mesh_.IsValid() )
			m_mesh_.Unload();
		if( m_material_.IsValid() )
			m_material_.Unload();
	}

	void cMeshComponent::SetMesh( const cShared_ptr< cAsset_Meta >& _mesh )
	{
		m_mesh_.SetAsset( _mesh );
	}

	void cMeshComponent::SetMaterial( const cShared_ptr< cAsset_Meta >& _material )
	{
		m_material_.SetAsset( _material );
	}
} // sk::Object::Components
