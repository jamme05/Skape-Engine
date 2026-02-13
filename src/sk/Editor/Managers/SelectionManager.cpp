//
// Created by willness on 2026-02-05.
// Copyright (c) 2026 William Ask S. Ness. All rights reserved.
//

#include "SelectionManager.h"

#include <sk/Editor/Components/EditorInternalComponent.h>
#include <sk/Scene/Object.h>

using namespace sk::Editor::Managers;

bool cSelectionManager::IsSelected( const Object::cObject& _object )
{
    auto& obj = const_cast< Object::cObject& >( _object );
    auto [ _, component ]
        = obj.AddOrGetInternalComponent< Components::cEditorInternalComponent >( m_expected_component_index_ );

    return component->m_selected_;
}

bool cSelectionManager::IsSelected( const Object::iComponent& _component ) const
{
    return m_selected_components_.contains( _component.GetUUID() );
}

bool cSelectionManager::IsSelected( const cAsset_Meta& _meta ) const
{
    return m_selected_assets_.contains( _meta.GetUUID() );
}

void cSelectionManager::AddSelectedObject( const cShared_ptr< Object::cObject >& _object, const bool _clear )
{
    if( _clear )
        Clear();

    auto [ _, component ]
        = _object->AddOrGetInternalComponent< Components::cEditorInternalComponent >( m_expected_component_index_ );

    component->m_selected_ = true;
    m_selected_objects_.emplace( _object->GetUUID(), _object );
}

void cSelectionManager::ToggleSelectedObject( const cShared_ptr< Object::cObject >& _object )
{
    auto [ _, component ]
        = _object->AddOrGetInternalComponent< Components::cEditorInternalComponent >( m_expected_component_index_ );

    if( component->m_selected_ )
        m_selected_objects_.erase( component->GetUUID() );
    else
        m_selected_objects_.emplace( _object->GetUUID(), _object );

    component->m_selected_ = !component->m_selected_;
}

void cSelectionManager::RemoveSelectedObject( const cUUID& _uuid )
{
    if( auto itr = m_selected_objects_.find( _uuid ); itr != m_selected_objects_.end() )
    {
        auto [ _, component ]
            = itr->second->AddOrGetInternalComponent< Components::cEditorInternalComponent >( m_expected_component_index_ );

        component->m_selected_ = false;
        m_selected_objects_.erase( itr );
    }
}

void cSelectionManager::AddSelectedComponent( const cShared_ptr< Object::iComponent >& _component, const bool _clear )
{
    if( _clear )
        Clear();

    m_selected_components_.emplace( _component->GetUUID(), _component );
}

void cSelectionManager::ToggleSelectedComponent( const cShared_ptr< Object::iComponent >& _component )
{
    if( const auto itr = m_selected_components_.find( _component->GetUUID() ); itr != m_selected_components_.end() )
        m_selected_components_.erase( _component->GetUUID() );
    else
        m_selected_components_.emplace( _component->GetUUID(), _component );
}

void cSelectionManager::RemoveSelectedComponent( const cUUID& _uuid )
{
    m_selected_components_.erase( _uuid );
}

void cSelectionManager::AddSelectedAsset( const cShared_ptr< cAsset_Meta >& _meta, bool _clear )
{
    if( _clear )
        m_selected_assets_.clear();

    m_selected_assets_.emplace( _meta->GetUUID(), _meta );
}

void cSelectionManager::ToggleSelectedAsset( const cShared_ptr< cAsset_Meta >& _meta )
{
    if( const auto itr = m_selected_assets_.find( _meta->GetUUID() ); itr != m_selected_assets_.end() )
        m_selected_assets_.erase( _meta->GetUUID() );
    else
        m_selected_assets_.emplace( _meta->GetUUID(), _meta );
}

void cSelectionManager::RemoveSelectedAsset( const cUUID& _uuid )
{
    m_selected_assets_.erase( _uuid );
}

void cSelectionManager::Clear()
{
    for( auto& [ _, obj ] : m_selected_objects_ )
    {
        auto [ __, component ]
            = obj->AddOrGetInternalComponent< Components::cEditorInternalComponent >( m_expected_component_index_ );

        component->m_selected_ = false;
    }

    m_selected_objects_.clear();
    m_selected_components_.clear();
}

void cSelectionManager::Clean()
{
    std::vector< hash< cUUID > > objects_to_remove{};
    std::vector< hash< cUUID > > components_to_remove{};
    for( auto& [ uuid, obj ] : m_selected_objects_ )
    {
        if( !obj.is_valid() )
            objects_to_remove.emplace_back( uuid );
    }

    for( auto& [ uuid, component ] : m_selected_components_ )
    {
        if( !component.is_valid() )
            components_to_remove.emplace_back( uuid );
    }

    for( auto& uuid : objects_to_remove )
        m_selected_objects_.erase( uuid );

    for( auto& uuid : components_to_remove )
        m_selected_components_.erase( uuid );
}
