/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <fastgltf/types.hpp>

#include "Assets/Asset.h"

#include "Containers/vector.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"


namespace sk::Assets
{
	class cMesh;
} // sk::Assets

namespace sk::Assets
{
	QW_ASSET_CLASS( Model )
	{
		SK_CLASS_BODY( Model )
	public:
		cModel( const std::string& _name );
		// TODO: Model class, aka container for meshes and a name.

		[[nodiscard]] auto& getTransform( void )       { return m_transform; }
		[[nodiscard]] auto& getTransform( void ) const { return m_transform; }

		[[nodiscard]] auto& getMeshes( void )       { return m_meshes; }
		[[nodiscard]] auto& getMeshes( void ) const { return m_meshes; }

	private:
		vector< cShared_ptr< cModel > > m_children;
		vector< cShared_ptr< cMesh > >  m_meshes;

		cTransform m_transform;
	};
} // sk::Assets::

REGISTER_CLASS( sk::Assets::Model )