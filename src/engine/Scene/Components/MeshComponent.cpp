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
	cMeshComponent::cMeshComponent( const cShared_ptr< cAsset_Meta >& _mesh )
	: m_mesh_( get_weak(), _mesh )
	, m_texture_( get_weak() )
	{
		m_mesh_.on_asset_loaded += []( const cAsset& _asset )
		{
			sk::println( "Asset with name: {} was loaded.", _asset.GetMeta()->GetName().view() );
		};
	}

	void cMeshComponent::render()
	{
		if( m_mesh_.IsLoaded() );
			// DoSomeInternalRender();
	}

	void cMeshComponent::enabled()
	{
		if( m_mesh_.IsValid() )
			m_mesh_.LoadAsync();
		if( m_texture_.IsValid() )
			m_texture_.LoadAsync();
	}

	void cMeshComponent::disabled()
	{
		if( m_mesh_.IsValid() )
			m_mesh_.Unload();
		if( m_texture_.IsValid() )
			m_texture_.Unload();
	}

	void cMeshComponent::SetMesh( const cShared_ptr< cAsset_Meta >& _mesh )
	{
		m_mesh_.SetAsset( _mesh );
	}

	void cMeshComponent::SetTexture( const cShared_ptr<cAsset_Meta>& _texture )
	{
		m_texture_.SetAsset( _texture );
	}
} // sk::Object::Components
