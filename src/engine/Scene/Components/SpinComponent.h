#pragma once
#include "Component.h"

namespace sk::Object::Components
{
    SK_COMPONENT_CLASS( SpinComponent )
    {
        SK_CLASS_BODY( SpinComponent )
    public:
        void update() override;
    };
} // sk::Object::Components::

DECLARE_CLASS( sk::Object::Components::SpinComponent )