
#pragma once

#include <sk/Math/Vector3.h>
#include <sk/Math/Vector4.h>

namespace sk
{
    class cSphere;

    class cAABox
    {
    public:
        enum eCorner : uint8_t
        {
            // Min
            kLeftBottomBack,
            kLeftTopBack,
            kRightTopBack,
            kRightBottomBack,
            // Max
            kRightTopFront,
            kLeftTopFront,
            kLeftBottomFront,
            kRightBottomFront,
        };

        cAABox() = default;
        cAABox( const cAABox& ) = default;
        cAABox( cAABox&& ) = default;
        cAABox( const cVector3f& _position, const cVector3f& _size );

        bool Contains( const cVector3f& _point ) const;
        bool Overlaps( const cAABox& _box ) const;
        bool Overlaps( const cSphere& _sphere ) const;

        auto operator[]( uint8_t _corner ) const -> cVector3f;

        cVector3f min = kZero;
        cVector3f max = kZero;
    };
} // sk::
