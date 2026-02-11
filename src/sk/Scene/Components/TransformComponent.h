/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Scene/Components/Component.h>

namespace sk::Object::Components
{
	SK_COMPONENT_CLASS( TransformComponent )
	{
		SK_CLASS_BODY( TransformComponent )
	sk_public:
		 cTransformComponent() = default;
		explicit cTransformComponent( const cShared_ptr< cSerializedObject >& _object );
		~cTransformComponent() override = default;

		auto Serialize() -> cShared_ptr< cSerializedObject > override;
	};
} // sk::Object::Components

SK_DECLARE_CLASS( sk::Object::Components::TransformComponent )