
#include "AABox.h"

#include <sk/Debugging/Macros/Assert.h>
#include <sk/Math/Sphere.h>

sk::cAABox::cAABox( const cVector3f& _position, const cVector3f& _size )
{
    const auto half_size = _size / 2;
    min = _position - half_size;
    max = _position + half_size;
}

bool sk::cAABox::Contains( const cVector3f& _point ) const
{
    for( int_fast8_t i = 0; i < 3; i++ )
    {
        if( _point[ i ] < min[ i ] || _point[ i ] > max[ i ] )
            return false;
    }

    return true;
}

bool sk::cAABox::Overlaps( const cAABox& _box ) const
{
    return max.x >= _box.min.x && _box.max.x >= min.x
        && max.y >= _box.min.y && _box.max.y >= min.y
        && max.z >= _box.min.z && _box.max.z >= min.z;
}

bool sk::cAABox::Overlaps( const cSphere& _sphere ) const
{
    return _sphere.Overlaps( *this );
}

auto sk::cAABox::operator[]( const uint8_t _corner ) const -> cVector3f
{
    switch( _corner )
    {
    case kLeftBottomBack:   return min;
    case kLeftBottomFront:  return cVector3f{ min.x, min.y, max.z };
    case kLeftTopBack:      return cVector3f{ min.x, max.y, min.z };
    case kLeftTopFront:     return cVector3f{ min.x, max.y, max.z };
    case kRightBottomBack:  return cVector3f{ max.x, min.y, min.z };
    case kRightBottomFront: return cVector3f{ max.x, min.y, max.z };
    case kRightTopBack:     return cVector3f{ max.x, max.y, min.z };
    case kRightTopFront:    return max;
    default: SK_BREAK; return {};
    }
}
