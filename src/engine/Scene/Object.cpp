/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Object.h"

#include "Managers/cLayer_Manager.h"

void sk::Object::iObject::SetLayer( const uint64_t _layer )
{
    auto& layer_manager = Scene::cLayer_Manager::get();
    m_layer_ = _layer;
    layer_manager.AddObject( get_shared() );
}

void sk::Object::iObject::SetRoot( const cShared_ptr< iComponent >& _new_root_component, const bool _override_parent )
{
    if( _override_parent )
        _new_root_component->SetParent( m_root->m_parent.Lock() );
    else if( _new_root_component->m_parent == m_root )
        _new_root_component->SetParent( nullptr );

    while( !m_root->m_children.empty() )
        m_root->m_children.front()->SetParent( _new_root_component );
    
    m_root = _new_root_component;
}
