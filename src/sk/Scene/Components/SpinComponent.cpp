
#include "SpinComponent.h"

#include <sk/Platform/Time.h>

using namespace sk::Object::Components;

cSpinComponent::cSpinComponent( cVector3f _speed )
: m_speed_( std::move( _speed ) )
{}

cSpinComponent::cSpinComponent( const cShared_ptr< cSerializedObject >& _object )
: cComponent( _object->GetBase< iComponent >() )
{
    _object->BeginRead( this );
    m_speed_ = _object->ReadData< cVector3f >( "speed" ).value_or( kZero );
    _object->EndRead();
}

void cSpinComponent::update()
{
    GetTransform().GetLocalRotation() += m_speed_ * Time::Delta;
    GetTransform().MarkDirty();

    for( auto& child : GetChildren() )
        child->GetTransform().MarkDirty();
}

sk::cShared_ptr< sk::cSerializedObject > cSpinComponent::Serialize()
{
    auto object = cSerializedObject::CreateForWrite( this );
    object->AddBase( cComponent::Serialize() );
    object->WriteData( "speed", cVector3d{ m_speed_ } );
    object->EndWrite();
    return object;
}
