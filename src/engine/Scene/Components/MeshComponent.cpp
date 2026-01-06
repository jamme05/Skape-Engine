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
	: m_mesh_( get_weak() )
	, m_material_( get_weak() )
	{
		m_mesh_.SetAsset( _mesh );
		m_material_.SetAsset( _material );
	}
	
	void cMeshComponent::enabled()
	{
		if( m_mesh_.IsValid() && !m_mesh_.IsLoaded() )
			m_mesh_.LoadAsync();
		if( m_material_.IsValid() && !m_material_.IsLoaded() )
			m_material_.LoadAsync();
	}

	void cMeshComponent::disabled()
	{
		if( m_mesh_.IsValid() )
			m_mesh_.Unload();
		if( m_material_.IsValid() )
			m_material_.Unload();
	}

	bool cMeshComponent::IsReady() const
	{
		return m_mesh_.IsLoaded() && m_material_.IsLoaded();
	}

	auto cMeshComponent::GetMesh() const -> Assets::cMesh*
	{
		return m_mesh_.GetAsset();
	}

	auto cMeshComponent::GetMaterial() const -> Assets::cMaterial*
	{
		return m_material_.GetAsset();
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
