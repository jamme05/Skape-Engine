/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Math/Matrix4x4.h>
#include <sk/Misc/Smart_Ptrs.h>

namespace sk
{
	// TODO: Add setter functions and provide a way to update the transforms children.
	class cTransform
	{
	public:
		explicit cTransform( cVector3f _position = kZero, cVector3f _rotation = kZero, cVector3f _scale = kOne )
		: m_position_( std::move( _position ) )
		, m_rotation_( std::move( _rotation ) )
		, m_scale_   ( std::move( _scale ) )
		{
			Update();
		}
		
		// TODO: Add some way to detect changes.
		// Something like a last_change being set to the current frame.

		// You need to manually mark the transform as dirty when calling this.
		[[ nodiscard ]]
		auto& GetLocalPosition()       { return m_position_; }
		[[ nodiscard ]]
		auto& GetLocalPosition() const { return m_position_; }
		void  SetLocalPosition( const cVector3f& _position );

		// You need to manually mark the transform as dirty when calling this.
		[[ nodiscard ]]
		auto& GetLocalRotation()       { return m_rotation_; }
		[[ nodiscard ]]
		auto& GetLocalRotation() const { return m_rotation_; }
		void  SetLocalRotation( const cVector3f& _rotation );

		// You need to manually mark the transform as dirty when calling this.
		[[ nodiscard ]]
		auto& GetLocalScale()       { return m_scale_; }
		[[ nodiscard ]]
		auto& GetLocalScale() const { return m_scale_; }
		void  SetLocalScale( const cVector3f& _scale );

		[[ nodiscard ]]
		constexpr auto& GetWorld() const { return m_world_; }

		[[ nodiscard ]]
		auto& GetRight() const { return reinterpret_cast< const cVector3f& >( m_world_.x ); }
		[[ nodiscard ]]
		auto& GetUp   () const { return reinterpret_cast< const cVector3f& >( m_world_.y ); }
		[[ nodiscard ]]
		auto& GetForward() const { return reinterpret_cast< const cVector3f& >( m_world_.z ); }
		[[ nodiscard ]]
		auto& GetPosition() const { return reinterpret_cast< const cVector3f& >( m_world_.w ); }

		[[ nodiscard ]]
		auto GetRightDir() const -> const cVector3f&;
		[[ nodiscard ]]
		auto GetUpDir   () const -> const cVector3f&;
		[[ nodiscard ]]
		auto GetForwardDir() const -> const cVector3f&;

		void SetParent( const cWeak_Ptr< cTransform >& _parent );

		// Note: It will set itself to dirty if any of its parents are marked as dirty.
		[[ nodiscard ]]
		bool IsDirty( bool _recursive = true ) const;
		void MarkDirty();

		void Update( bool _force = false );

		void CacheNormalized();

	private:
		cWeak_Ptr< cTransform > m_parent_ = nullptr;

		cMatrix4x4f m_world_;

		cVector3f m_world_right_;
		cVector3f m_world_up_;
		cVector3f m_world_forward_;

		cVector3f m_position_;
		cVector3f m_rotation_;
		cVector3f m_scale_;
		
		bool m_is_dirty_       = true;
		bool m_has_normalized_ = false;
	};
} // sk::

