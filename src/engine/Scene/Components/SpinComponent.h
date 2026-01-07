#pragma once
#include "Component.h"

namespace sk::Object::Components
{
    SK_COMPONENT_CLASS( SpinComponent )
    {
        SK_CLASS_BODY( SpinComponent )
    public:
        explicit cSpinComponent( cVector3f _speed = kUp );
        void update() override;
        
    private:
        cVector3f m_speed_;
    };
} // sk::Object::Components::

DECLARE_CLASS( sk::Object::Components::SpinComponent )