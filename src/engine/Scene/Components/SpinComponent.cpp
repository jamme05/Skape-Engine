#include "SpinComponent.h"

using namespace sk::Object::Components;

void cSpinComponent::update()
{
    GetTransform().getRotation() += { 0.0f, 0.1f, 0.0f };
    GetTransform().update();

    for( auto& child : m_children )
        child->GetTransform().update();
}
