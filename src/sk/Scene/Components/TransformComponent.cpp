/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "TransformComponent.h"

using namespace sk::Object;

Components::cTransformComponent::cTransformComponent( cSerializedObject& _object )
: cComponent( _object.GetBase< iComponent >().value() )
{}

auto Components::cTransformComponent::Serialize() -> cSerializedObject
{
    cSerializedObject object( this );
    object.AddBase( iComponent::Serialize() );
    object.EndWrite();
    return object;
}
