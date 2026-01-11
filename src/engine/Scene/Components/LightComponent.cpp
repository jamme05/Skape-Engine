

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
        fix_data();
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
    ASSIGN_SETTING( type, _type );
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
    switch( GetType() )
    {
    case eType::kDirectional:
        fix_directional_data();
    break;
    case eType::kPoint:
        fix_point_data();
    break;
    case eType::kSpot:
        fix_spot_data();
    break;
    }
}

void cLightComponent::fix_directional_data()
{
    sDirectionalLight light;
    
    light.color     = m_settings_.color * m_settings_.intensity;
    light.direction = GetTransform().GetWorldFront();
    light.shadow_cast_index = -1;
    
    m_data_ = light;
}

void cLightComponent::fix_point_data()
{
    sPointLight light;
    
    light.color    = m_settings_.color * m_settings_.intensity * m_settings_.color.a;
    light.position = GetTransform().GetWorldPosition();
    light.radius   = m_settings_.radius;
    light.shadow_cast_index = -1;
    
    m_data_ = light;
}

void cLightComponent::fix_spot_data()
{
    sSpotLight light;
    
    light.color       = m_settings_.color * m_settings_.intensity * m_settings_.color.a;
    light.position    = GetTransform().GetWorldPosition();
    light.direction   = GetTransform().GetWorldFront();
    light.inner_angle = m_settings_.inner_angle;
    light.outer_angle = m_settings_.outer_angle;
    light.shadow_cast_index = -1;
    
    m_data_ = light;
}

void cLightComponent::update_data()
{
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
}
