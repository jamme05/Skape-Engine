
#pragma once

#include <sk/Scene/Components/Component.h>

#include <set>

namespace sk::Editor::Components
{
    SK_COMPONENT_CLASS( EditorInternalComponent )
    {
        SK_CLASS_BODY( EditorInternalComponent )
    public:
        bool selected = false;
        std::set< cUUID > selected_components;
    };
} // sk::Editor::Components::

SK_DECLARE_CLASS( sk::Editor::Components::EditorInternalComponent )