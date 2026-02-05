//
// Created by willness on 2026-02-05.
// Copyright (c) 2026 William Ask S. Ness. All rights reserved.
//


#pragma once

#include <sk/Containers/Map.h>
#include <sk/Misc/Singleton.h>
#include <sk/Misc/UUID.h>
#include <sk/Scene/Object.h>

namespace sk::Object
{
    class iObject;
}

namespace sk::Editor::Managers
{
    class cSelectionManager : public cSingleton< cSelectionManager >
    {
    public:
        [[ nodiscard ]] auto& GetObjects   () const { return m_selected_objects_;    }
        [[ nodiscard ]] auto& GetComponents() const { return m_selected_components_; }

        bool IsSelected( const Object::iObject& _object );
        bool IsSelected( const Object::iComponent& _component ) const;

        void AddSelectedObject   ( const cShared_ptr< Object::iObject >& _object, bool _clear );
        void ToggleSelectedObject( const cShared_ptr< Object::iObject >& _object );
        void RemoveSelectedObject( const cUUID& _uuid );

        void AddSelectedComponent   ( const cShared_ptr< Object::iComponent >& _component, bool _clear );
        void ToggleSelectedComponent( const cShared_ptr< Object::iComponent >& _component );
        void RemoveSelectedComponent( const cUUID& _uuid );

        void Clear();
        void Clean();
    private:
        using object_map_t    = unordered_map< hash< cUUID >, cWeak_Ptr< Object::iObject > >;
        using component_map_t = unordered_map< hash< cUUID >, cWeak_Ptr< Object::iComponent > >;

        size_t          m_expected_component_index_ = std::numeric_limits< size_t >::max();
        object_map_t    m_selected_objects_;
        component_map_t m_selected_components_;
    };
} // sk::Editor::Managers::