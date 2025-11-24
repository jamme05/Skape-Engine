/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <fastgltf/types.hpp>

#include "Assets/Asset.h"

#include "Containers/Vector.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"

namespace sk::Assets
{
	class cMesh;
} // sk::Assets

namespace sk::Assets
{
	SK_ASSET_CLASS( Model )
	{
		SK_CLASS_BODY( Model )
	public:
		cModel();
		// TODO: Model class, aka container for meshes and a name.

		[[nodiscard]] auto& getMeshes( void ) const { return m_meshes; }
		// SK_PUBLIC_FUNCTION( getMeshes )
	private:
		vector< cShared_ptr< cModel > > m_children;
		vector< cShared_ptr< cMesh > >  m_meshes;
	};
} // sk::Assets::

DECLARE_CLASS( sk::Assets::Model )
