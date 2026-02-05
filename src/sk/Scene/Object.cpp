/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Object.h"

#include <sk/Scene/Managers/Layer_Manager.h>

sk::Object::iObject::iObject( const std::string& _name )
: m_name( _name )
{
    m_root = AddComponent< Components::cTransformComponent >();
    SetLayer( 0 );
}

sk::Object::iObject::~iObject()
{
    Scene::cLayer_Manager::get().RemoveObject( get_shared() );
    m_children_.clear();
    m_components_.clear();
    m_root = nullptr;
}

void sk::Object::iObject::render()
{
    // TODO: Add actual event vector or something.
    for( auto& val : m_components_ | std::views::values )
    {
        val->PostEvent( kRender );
        val->PostEvent( kDebugRender );
    }
}

void sk::Object::iObject::update()
{
    for( auto& val : m_components_ | std::views::values )
        val->PostEvent( kUpdate );
}

void sk::Object::iObject::SetLayer( const uint64_t _layer )
{
    auto& layer_manager = Scene::cLayer_Manager::get();
    m_layer_ = _layer;
    layer_manager.AddObject( get_shared() );
}

void sk::Object::iObject::SetRoot( const cShared_ptr< iComponent >& _new_root_component, const bool _override_parent )
{
    if( _override_parent )
        _new_root_component->SetParent( m_root->m_parent_.Lock() );
    else if( _new_root_component->m_parent_ == m_root )
        _new_root_component->SetParent( nullptr );

    while( !m_root->m_children_.empty() )
        m_root->m_children_.front()->SetParent( _new_root_component );
    
    m_root = _new_root_component;
}

void sk::Object::iObject::registerRecursive()
{
    for( auto& child : m_children_ )
        child->registerRecursive();

    m_root->registerRecursive();
}

void sk::Object::iObject::enableRecursive()
{
    for( auto& child : m_children_ )
        child->enableRecursive();

    for( auto& component : m_components_ | std::views::values )
    {
        if( component->m_enabled_ )
            component->enableRecursive();
    }
}

void sk::Object::iObject::disableRecursive()
{

}
