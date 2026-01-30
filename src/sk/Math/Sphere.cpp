
#include "Sphere.h"

#include <sk/Math/AABox.h>
#include <sk/Math/Transform.h>

sk::cSphere::cSphere( const float _radius, const cVector3f& _position )
: position( _position )
, radius( _radius )
{}

void sk::cSphere::ResizeToFit( const cVector3f& _point )
{
    const auto dist2 = ( _point - position ).dot();

    if( dist2 < Math::square( radius ) )
        return;

    radius = Math::sqrt( dist2 );
}

void sk::cSphere::ResizeToFit( const cAABox& _box )
{
    for( int_fast8_t i = 0; i < 8; i++ )
        ResizeToFit( _box[ i ] );
}

bool sk::cSphere::Contains( const cVector3f& _point ) const
{
    return ( _point - position ).dot() < Math::square( radius );
}

bool sk::cSphere::Overlaps( const cSphere& _sphere ) const
{
    const auto total_radius2 = Math::square( radius + _sphere.radius );

    return ( _sphere.position - position ).dot() < total_radius2;
}

bool sk::cSphere::Overlaps( const cAABox& _box ) const
{
    float d_min = 0.0f;
    for( int_fast8_t i = 0; i < 3; i++ )
    {
        if( position[ i ] < _box.min[ i ] )
            d_min += Math::square( position[ i ] - _box.min[ i ] );
        else if( position[ i ] > _box.max[ i ] )
            d_min += Math::square( position[ i ] - _box.max[ i ] );
    }

    return d_min < Math::square( radius );
}

auto sk::cSphere::Transform( const cTransform& _transform ) -> cSphere&
{
    return Transform( _transform.GetWorld() );
}

auto sk::cSphere::Transform( const cMatrix4x4f& _matrix ) -> cSphere&
{
    auto longest_scale2 = _matrix.right.dot();
    if( const auto up_scale2 = _matrix.up.dot(); up_scale2 > longest_scale2 )
        longest_scale2 = up_scale2;
    if( const auto forward_scale_2 = _matrix.right.dot(); forward_scale_2 > longest_scale2 )
        longest_scale2 = forward_scale_2;

    radius *= longest_scale2;

    _matrix.Transform( position );

    return *this;

}
