

#include "Component.h"

using namespace sk::Object;

iComponent::~iComponent()
{
    m_object_ = nullptr;
    m_children_.clear();
}

void iComponent::SetParent( const cShared_ptr< iComponent >& _component )
{
    if( m_parent_ && !m_parent_.Lock()->m_children_.empty() )
    {
        if( const auto itr = std::ranges::find( m_parent_->m_children_, m_self_.Lock() ); itr != m_parent_->m_children_.end() )
            m_parent_->m_children_.erase( itr );
    }
    m_parent_ = _component;
    if( _component )
        _component->m_children_.emplace_back( m_self_.Lock() );
    m_transform_->SetParent( ( _component != nullptr ) ? _component->m_transform_ : nullptr );
}
void iComponent::SetObject( const cWeak_Ptr< cObject >& _parent_object )
{
    for( auto& child : m_children_ )
        child->SetObject( _parent_object );

    m_object_ = _parent_object;
}

void iComponent::registerRecursive()
{
    m_transform_->Update();
    for( auto& child : m_children_ )
        child->registerRecursive();
    registerEvents();
}

void iComponent::enableRecursive()
{
    for( auto& child : m_children_ )
    {
        if( child->m_enabled_ )
            child->enableRecursive();
    }
    enabled();
}

void iComponent::disableRecursive()
{
    for( auto& child : m_children_ )
        child->disableRecursive();
}
