
#include "Transform.h"

using namespace sk;

void cTransform::SetPosition( const cVector3f& _position )
{
    m_position_ = _position;
    
    MarkDirty();
}

void cTransform::SetRotation( const cVector3f& _rotation )
{
    m_rotation_ = _rotation;
    
    MarkDirty();
}

void cTransform::SetScale( const cVector3f& _scale )
{
    m_scale_ = _scale;
    
    MarkDirty();
}

void cTransform::SetParent( const cWeak_Ptr< cTransform >& _parent )
{
    m_parent_ = _parent;
    
    MarkDirty();
}

bool cTransform::IsDirty( const bool _recursive ) const
{
    // Should we keep this function const, even if we cache the result using const cast?
    // To the user this should act and look as a getter.
    // But caching will make it significantly more efficient as we skip the recursion.

    if( m_is_dirty_ )
        return true;
    
    if( _recursive && m_parent_.is_valid() )
    {
        // Cache the result
        const_cast< bool& >( m_is_dirty_ ) = m_parent_->IsDirty();
        return m_is_dirty_;
    }
    
    return false;
}

void cTransform::MarkDirty()
{
    m_is_dirty_ = true;
}

void cTransform::Update( const bool _force )
{
    // We skip the IsDirty call on this object as we will reset m_is_dirty_ in the end anyway.
    // This method will be slightly more efficient.
    
    const bool parent_dirty = m_parent_.is_valid() && m_parent_->IsDirty();
    
    // Already up to date
    if( !_force && !m_is_dirty_ && !parent_dirty )
        return;
    
    const auto local = Math::Matrix4x4::scale_rotate_translate(
        m_scale_, m_rotation_ * Math::kDegToRad< float >, m_position_ );

    if( m_parent_.is_valid() )
    {
        if( parent_dirty )
            m_parent_->Update();
        m_world_ = local * m_parent_->GetWorld();
    }
    else
        m_world_ = local;
    
    m_is_dirty_ = false;
}
