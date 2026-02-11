/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Scene/Object.h>
#include <sk/Scene/Components/CameraComponent.h>

namespace sk::Object
{
	SK_OBJECT_CLASS( Camera )
	{
		SK_CLASS_BODY( Camera )
	public:
		explicit cCamera( const std::string& _name );

		void setAsMain() const;

	protected:
		cShared_ptr< Components::cCameraComponent > m_camera_;
	};
} // sk::Object

SK_DECLARE_CLASS( sk::Object::Camera )