

#include "Light_Manager.h"

#include <Scene/Components/LightComponent.h>

namespace 
{
    // From: https://en.cppreference.com/w/cpp/utility/variant/visit
    template< class... Ts >
    struct sVisitor : Ts... { using Ts::operator()...; };
} // ::

sk::Scene::cLight_Manager::cLight_Manager()
: m_light_settings_buffer_( "Light Settings Buffer", false )
, m_directional_buffer_( "Directional Light Buffer", false )
, m_point_buffer_( "Point Light Buffer", false )
, m_spot_buffer_( "Spot Light Buffer", false )
, m_shadow_caster_buffer_( "Shadow Caster Buffer", false )
{
}

auto sk::Scene::cLight_Manager::GetLights() const -> const light_vec_t&
{
    return m_lights_;
}

auto sk::Scene::cLight_Manager::GetShadowCasters() const -> const light_vec_t&
{
    return m_shadow_casters_;
}

void sk::Scene::cLight_Manager::register_light( light_ptr_t _light )
{
    m_lights_.emplace_back( _light );
    _light->m_registered_index_ = static_cast< uint32_t >( m_lights_.size() - 1 );
    
    sVisitor visitor{
        [ & ]( const Light::sDirectionalLight& _data )
        {
            _light->m_data_index_ = static_cast< uint32_t >( m_directional_buffer_.EmplaceBack( _data ) );
        },
        [ & ]( const Light::sPointLight& _data )
        {
            _light->m_data_index_ = static_cast< uint32_t >( m_point_buffer_.EmplaceBack( _data ) );
        },
        [ & ]( const Light::sSpotLight& _data )
        {
            _light->m_data_index_ = static_cast< uint32_t >( m_spot_buffer_.EmplaceBack( _data ) );
        }
    };
    
    std::visit( visitor, _light->GetData() );
    
    if( _light->GetSettings().casts_shadows )
        add_shadow_caster( _light );
}

void sk::Scene::cLight_Manager::update_light( light_ptr_t _light, const Light::sSettings& _previous_settings )
{
    const bool was_shadow_caster = _previous_settings.casts_shadows;
    if( const bool casts_shadows = _light->GetSettings().casts_shadows; was_shadow_caster && !casts_shadows )
    {
        // No longer a shadow caster, remove
        remove_shadow_caster( _light );
    }
    else if( !was_shadow_caster && casts_shadows )
    {
        // New shadow caster, add
        add_shadow_caster( _light );
    }
}

void sk::Scene::cLight_Manager::unregister_light( light_ptr_t _light )
{
    if( const auto index = _light->m_data_index_; index != m_lights_.size() - 1 )
    {
        auto& back = m_lights_.back();
        
        back->m_data_index_ = index;
        std::swap( m_lights_[ index ], back );
    }
    
    m_lights_.pop_back();
    
    _light->m_data_index_ = std::numeric_limits< uint32_t >::max();
    
    if( _light->GetSettings().casts_shadows )
        remove_shadow_caster( _light );
}

void sk::Scene::cLight_Manager::add_shadow_caster( const light_ptr_t& _light )
{
    m_shadow_casters_.emplace_back( _light );
    _light->m_shadow_data_index_ = static_cast< uint32_t >( m_shadow_casters_.size() - 1 );
}

void sk::Scene::cLight_Manager::remove_shadow_caster( const light_ptr_t& _light )
{
    if( const auto index = _light->m_shadow_data_index_; index != m_shadow_casters_.size() - 1 )
    {
        auto& back = m_shadow_casters_.back();
        
        back->m_shadow_data_index_ = index;
        std::swap( m_shadow_casters_[ index ], back );
    }
    
    m_shadow_casters_.pop_back();
    
    _light->m_shadow_data_index_ = std::numeric_limits< uint32_t >::max();
}
