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
		 cTransformComponent( void ) = default;
		~cTransformComponent( void ) override = default;
	};
} // sk::Object::Components

DECLARE_CLASS( sk::Object::Components::TransformComponent )