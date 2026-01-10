

#include "LightComponent.h"

#include "Scene/Managers/Light_Manager.h"

using namespace sk::Object::Components;

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
    
}

auto cLightComponent::GetSettings() const -> const settings_t&
{
    return m_settings_;
}

void cLightComponent::SetSettings( const settings_t& _settings )
{
    const auto prev_settings = m_settings_;
    m_settings_ = _settings;
    
    Scene::cLight_Manager::get().update_light( get_weak().Cast< cLightComponent >(), prev_settings );
}

auto cLightComponent::GetType() const -> type_t
{
    return m_settings_.type;
}

void cLightComponent::SetType( const type_t _type )
{
    // TODO: Have changing the type update the settings.
    m_settings_.type = _type;
}

auto cLightComponent::GetViewProjMatrix() const -> const cMatrix4x4f&
{
    return m_view_proj_matrix_;
}
