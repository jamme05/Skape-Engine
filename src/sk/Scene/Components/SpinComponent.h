

#pragma once

#include "Component.h"

namespace sk::Object::Components
{
    SK_COMPONENT_CLASS( SpinComponent )
    {
        SK_CLASS_BODY( SpinComponent )
    public:
        explicit cSpinComponent( cVector3f _speed = kUp );
        explicit cSpinComponent( cSerializedObject& _object );
        void update() override;

        auto Serialize() -> cSerializedObject override;

    private:
        cVector3f m_speed_;
    };
} // sk::Object::Components::

SK_DECLARE_CLASS( sk::Object::Components::SpinComponent )