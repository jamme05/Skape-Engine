/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "TransformComponent.h"

using namespace sk::Object;

Components::cTransformComponent::cTransformComponent( const cShared_ptr< cSerializedObject >& _object )
: cComponent( _object->GetBase< iComponent >() )
{}

auto Components::cTransformComponent::Serialize() -> cShared_ptr< cSerializedObject >
{
    auto object = cSerializedObject::CreateForWrite( this );
    object->AddBase( iComponent::Serialize() );
    object->EndWrite();
    return object;
}
