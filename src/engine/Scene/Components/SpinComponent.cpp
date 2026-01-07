#include "SpinComponent.h"

#include "Platform/Time.h"

using namespace sk::Object::Components;

cSpinComponent::cSpinComponent( cVector3f _speed )
: m_speed_( std::move( _speed ) )
{}

void cSpinComponent::update()
{
    GetTransform().getRotation() += m_speed_ * Time::Delta;
    GetTransform().update();

    for( auto& child : m_children )
        child->GetTransform().update();
}
