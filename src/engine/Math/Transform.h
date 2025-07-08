/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Matrix4x4.h"

namespace sk
{
	class cTransform
	{
	public:
		cTransform( const cVector3f& _position = Math::kZero, const cVector3f& _rotation = Math::kZero, const cVector3f& _scale = Math::kOne )
		: m_position( _position )
		, m_rotation( _rotation )
		, m_scale   ( _scale )
		{
			update();
		}

		auto& getPosition( void )       { return m_position; }
		auto& getPosition( void ) const { return m_position; }

		auto& getRotation( void )       { return m_rotation; }
		auto& getRotation( void ) const { return m_rotation; }

		auto& getScale   ( void )       { return m_scale; }
		auto& getScale   ( void ) const { return m_scale; }

		constexpr auto& getWorld( void ) const { return m_world; }

		auto& getWorldFront( void ) const { return reinterpret_cast< const cVector3f& >( m_world.z ); }
		auto& getWorldRight( void ) const { return reinterpret_cast< const cVector3f& >( m_world.x ); }
		auto& getWorldUp   ( void ) const { return reinterpret_cast< const cVector3f& >( m_world.y ); }

		auto& getWorldPosition( void ) const { return reinterpret_cast< const cVector3f& >( m_world.w ); }

		void setParent( const cTransform& _parent ){ m_parent = &_parent; }
		void setParent( const cTransform* _parent ){ m_parent =  _parent; }

		// TODO: Move to cpp file
		cTransform& update( void )
		{
			const auto local = Math::Matrix4x4::scale_rotate_translate(
				m_scale, m_rotation * Math::kDegToRad< float >, m_position );

			if( m_parent )
				m_world = m_parent->getWorld() * local;
			else
				m_world = local;

			return *this;
		} // update

	private:
		// TODO: Use the local matrix and remove the direct access.
		cVector3f m_position;
		cVector3f m_rotation;
		cVector3f m_scale;

		cMatrix4x4f m_world;

		const cTransform* m_parent = nullptr;
	};
} // sk::

