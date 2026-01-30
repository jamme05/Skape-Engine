
#pragma once

#include <sk/Math/Matrix4x4.h>
#include <sk/Math/Vector3.h>
#include <sk/Math/Vector4.h>

namespace sk
{
    class cTransform;
    class cAABox;

    class cSphere
    {
    public:
        cSphere() = default;
        cSphere( const cSphere& ) = default;
        cSphere( cSphere&& ) = default;
        explicit cSphere( float _radius, const cVector3f& _position = kZero );

        auto operator=( const cSphere& ) -> cSphere& = default;
        auto operator=( cSphere&& ) noexcept -> cSphere& = default;
        bool operator==( const cSphere& _other ) const = default;

        void ResizeToFit( const cVector3f& _point );
        void ResizeToFit( const cAABox& _box );

        bool Contains( const cVector3f& _point ) const;
        bool Overlaps( const cSphere& _sphere ) const;
        bool Overlaps( const cAABox& _box ) const;

        auto Transform( const cTransform& _transform ) -> cSphere&;
        auto Transform( const cMatrix4x4f& _matrix ) -> cSphere&;

        cVector3f position = kZero;
        float     radius   = 0.0f;
    };
} // sk::
