

#include "LightComponent.h"

#include "Scene/Managers/Light_Manager.h"

using namespace sk::Object::Components;
using namespace sk::Scene::Light;

namespace 
{
    // TODO: Make an include for this
    // From: https://en.cppreference.com/w/cpp/utility/variant/visit
    template< class... Ts >
    struct sVisitor : Ts... { using Ts::operator()...; };
} // ::

cLightComponent::cLightComponent( const settings_t& _settings )
: m_settings_( _settings )
{
    SK_BREAK_IF( sk::Severity::kEngine, m_settings_.type != type_t::kDirectional,
        "Warning: Only directional lights are supported at the moment" )

    init_data();
    fix_data();
    
    Scene::cLight_Manager::get().register_light( get_weak().Cast< cLightComponent >() );
}

cLightComponent::~cLightComponent()
{
    Scene::cLight_Manager::get().unregister_light( get_weak().Cast< cLightComponent >() );
}

void cLightComponent::update()
{
    if( m_transform_->IsDirty() )
    {
        m_transform_->Update();
        update_data();
    }
}

auto cLightComponent::GetSettings() const -> const settings_t&
{
    return m_settings_;
}

auto cLightComponent::GetType() const -> type_t
{
    return m_settings_.type;
}

#define ASSIGN_SETTING_0_( Setting, NewValue ) \
    const auto prev_settings = m_settings_;   \
    Setting = NewValue; \
    Scene::cLight_Manager::get().update_light( get_weak().Cast< cLightComponent >(), prev_settings ); \
    fix_data()

#define ASSIGN_SETTING( Setting, ... ) \
    ASSIGN_SETTING_0_( FIRST( __VA_OPT__( m_settings_ . Setting , ) m_settings_ ), FIRST( __VA_OPT__( __VA_ARGS__, ) Setting ) )

void cLightComponent::SetSettings( const settings_t& _settings )
{
    ASSIGN_SETTING( _settings );
}

void cLightComponent::SetType( const type_t _type )
{
    if( GetType() != _type )
    {
        init_data();
        fix_data();
        
        ASSIGN_SETTING( type, _type );
    }
}

auto cLightComponent::GetData() const -> const data_t&
{
    return m_data_;
}

auto cLightComponent::GetViewProjMatrix() const -> const cMatrix4x4f&
{
    return m_view_proj_matrix_;
}

void cLightComponent::fix_data()
{
    Scene::cLight_Manager::get().mark_buffer_dirty( GetType() );
    
    switch( GetType() )
    {
    case eType::kDirectional:
    {
        auto& data = m_data_ptr_.index() == 0
            ? std::get< sDirectionalLight >( m_data_ )
            : *std::get< sDirectionalLight* >( m_data_ptr_ );
            
        fix_directional_data( data );
    }
    break;
    case eType::kPoint:
    {
        auto& data = m_data_ptr_.index() == 0
            ? std::get< sPointLight >( m_data_ )
            : *std::get< sPointLight* >( m_data_ptr_ );

        fix_point_data( data );
    }
    break;
    case eType::kSpot:
    {
        auto& data = m_data_ptr_.index() == 0
            ? std::get< sSpotLight >( m_data_ )
            : *std::get< sSpotLight* >( m_data_ptr_ );

        fix_spot_data( data );
    }
    break;
    }
}

void cLightComponent::fix_directional_data( sDirectionalLight& _data ) const
{
    _data.color     = m_settings_.color * m_settings_.intensity;
    _data.direction = GetTransform().GetWorldFront();
    _data.shadow_cast_index = -1;
}

void cLightComponent::fix_point_data( sPointLight& _data ) const
{
    _data.color    = m_settings_.color * m_settings_.intensity * m_settings_.color.a;
    _data.position = GetTransform().GetWorldPosition();
    _data.radius   = m_settings_.radius;
    _data.shadow_cast_index = -1;
}

void cLightComponent::fix_spot_data( sSpotLight& _data ) const
{
    _data.color       = m_settings_.color * m_settings_.intensity * m_settings_.color.a;
    _data.position    = GetTransform().GetWorldPosition();
    _data.direction   = GetTransform().GetWorldFront();
    _data.inner_angle = m_settings_.inner_angle;
    _data.outer_angle = m_settings_.outer_angle;
    _data.shadow_cast_index = -1;
}

void cLightComponent::update_data()
{
    auto& manager = Scene::cLight_Manager::get();
    
    auto& transform = GetTransform();
    auto& settings  = m_settings_;
    sVisitor visitor{
        [ &transform ]( sDirectionalLight& _light )
        {
            _light.direction = transform.GetWorldFront();
        },
        [ &transform ]( sPointLight& _light )
        {
            _light.position = transform.GetWorldPosition();
        },
        [ &transform, &settings ]( sSpotLight& _light )
        {
            _light.position  = transform.GetWorldPosition();
            _light.direction = transform.GetWorldFront() * settings.radius;
        }
    };
    
    std::visit( visitor, m_data_ );
    
    manager.mark_buffer_dirty( GetType() );
    
    if( m_shadow_info_ != nullptr )
    {
        m_shadow_info_->light_matrix = GetViewProjMatrix();
        manager.mark_shadow_buffer_dirty();
    }
}

void cLightComponent::init_data()
{
    switch( m_settings_.type )
    {
    case eType::kDirectional:
        m_data_ = sDirectionalLight{};
        break;
    case eType::kPoint:
        m_data_ = sPointLight{};
        break;
    case eType::kSpot:
        m_data_ = sSpotLight{};
        break;
    }
}
