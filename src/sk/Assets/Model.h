/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Asset.h>
#include <sk/Containers/Vector.h>
#include <sk/Math/Transform.h>
#include <sk/Math/Vector4.h>

#include <fastgltf/types.hpp>

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

SK_DECLARE_CLASS( sk::Assets::Model )
