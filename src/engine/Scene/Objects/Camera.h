/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Scene/Object.h"
#include "Scene/Components/CameraComponent.h"

namespace sk::Object
{
	QW_OBJECT_CLASS( Camera )
	{
	public:
		explicit cCamera( const std::string& _name );

		void setAsMain( void ) const;

		Components::CameraComponent::ptr_t m_camera;
	};
} // sk::Object

