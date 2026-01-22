

#pragma once

#include <sk/Scene/Components/Component.h>

namespace sk::Scene
{
    class cLayer_Manager;
} // sk::Scene

namespace sk::Object::Components
{
    SK_COMPONENT_CLASS( Layer_Info_Component )
    {
        friend class sk::Scene::cLayer_Manager;
        size_t m_layer_index_ = std::numeric_limits< size_t >::max();
        size_t m_index_       = std::numeric_limits< size_t >::max();
    };
} // sk::Object::Components
