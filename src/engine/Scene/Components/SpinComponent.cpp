#include "SpinComponent.h"

#include "Platform/Time.h"

using namespace sk::Object::Components;

cSpinComponent::cSpinComponent( cVector3f _speed )
: m_speed_( std::move( _speed ) )
{}

void cSpinComponent::update()
{
    GetTransform().GetRotation() += m_speed_ * Time::Delta;
    GetTransform().MarkDirty();

    for( auto& child : m_children_ )
        child->GetTransform().MarkDirty();
}
