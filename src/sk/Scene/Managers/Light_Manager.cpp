

#include "Light_Manager.h"

#include <sk/Scene/Components/LightComponent.h>

#include "sk/Graphics/Rendering/Render_Target.h"

namespace 
{
    // From: https://en.cppreference.com/w/cpp/utility/variant/visit
    template< class... Ts >
    struct sVisitor : Ts... { using Ts::operator()...; };
} // ::

sk::Scene::cLight_Manager::cLight_Manager()
: m_light_settings_buffer_( "Light Settings Buffer", 1, false )
, m_shadow_caster_buffer_( "Shadow Caster Buffer", false )
, m_directional_buffer_( "Directional Light Buffer", false )
, m_point_buffer_( "Point Light Buffer", false )
, m_spot_buffer_( "Spot Light Buffer", false )
{
    // TODO: Dynamic atlas size.
    m_atlas_size_ = 512;
}

void sk::Scene::cLight_Manager::Update()
{
    auto& settings = *m_light_settings_buffer_;
    settings.directional_light_count = static_cast< uint32_t >( m_directional_light_indices_.size() );
    settings.point_light_count       = static_cast< uint32_t >( m_point_light_indices_      .size() );
    settings.spot_light_count        = static_cast< uint32_t >( m_spot_light_indices_       .size() );
    settings.uses_extended           = 0;

    if( m_directional_buffer_.IsDirty() )
    {
        if( settings.directional_light_count <= sLightSettings::kConstantDirectionalMax )
            std::copy_n( m_directional_buffer_.Data(), settings.directional_light_count, settings.constant_directional_light );
        else
        {
            settings.uses_extended |= 0x01;
            m_directional_buffer_.Upload();
        }
    }

    if( m_point_buffer_.IsDirty() )
    {
        if( settings.point_light_count <= sLightSettings::kConstantPointMax )
            std::copy_n( m_point_buffer_.Data(), settings.point_light_count, settings.constant_point_light );
        else
        {
            settings.uses_extended |= 0x02;
            m_point_buffer_.Upload();
        }
    }

    if( m_spot_buffer_.IsDirty() )
    {
        if( settings.spot_light_count <= sLightSettings::kConstantSpotMax )
            std::copy_n( m_spot_buffer_.Data(), settings.spot_light_count, settings.constant_spot_light );
        else
        {
            settings.uses_extended |= 0x04;
            m_spot_buffer_.Upload();
        }
    }

    if( !m_preserve_atlas_ )
        compute_atlas();

    m_shadow_caster_buffer_.Upload();
}

auto sk::Scene::cLight_Manager::GetAtlasSize() const -> cVector2u32
{
    return m_computed_atlas_size_;
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
    _light->m_registered_index_ = static_cast< uint32_t >( m_lights_.size() );
    m_lights_.emplace_back( _light );
    
    sVisitor visitor{
        [ & ]( const Light::sDirectionalLight& _data )
        {
            m_directional_light_indices_.emplace_back( _light->m_registered_index_ );
            _light->m_data_index_ = static_cast< uint32_t >( m_directional_buffer_.EmplaceBack( _data ) );
        },
        [ & ]( const Light::sPointLight& _data )
        {
            m_point_light_indices_.emplace_back( _light->m_registered_index_ );
            _light->m_data_index_ = static_cast< uint32_t >( m_point_buffer_.EmplaceBack( _data ) );
        },
        [ & ]( const Light::sSpotLight& _data )
        {
            m_spot_light_indices_.emplace_back( _light->m_registered_index_ );
            _light->m_data_index_ = static_cast< uint32_t >( m_spot_buffer_.EmplaceBack( _data ) );
        }
    };
    
    std::visit( visitor, _light->GetData() );
    
    if( _light->GetSettings().casts_shadows )
        add_shadow_caster( _light );
}

void sk::Scene::cLight_Manager::update_light( light_ptr_t _light, const Light::sSettings& _previous_settings )
{
    if( _light->GetType() != _previous_settings.type )
    {
        // TODO: Check if this can be done in a faster way.
        unregister_light( _light );
        register_light( _light );
        
        return;
    }

    auto& settings = _light->GetSettings();
    
    const bool was_shadow_caster = _previous_settings.casts_shadows;
    if( const bool casts_shadows = settings.casts_shadows; was_shadow_caster && !casts_shadows )
    {
        // No longer a shadow caster, remove
        remove_shadow_caster( _light );
    }
    else if( !was_shadow_caster && casts_shadows )
    {
        // New shadow caster, add
        add_shadow_caster( _light );
    }

    if( was_shadow_caster && settings.casts_shadows && _previous_settings.shadow_resolution != settings.shadow_resolution )
    {
        if( !Memory::is_power_of_two( settings.shadow_resolution ) )
        {
            SK_BREAK;
            _light->m_settings_.shadow_resolution = Math::roundToPow2( settings.shadow_resolution );
        }
        m_preserve_atlas_ = false;
    }
}

void sk::Scene::cLight_Manager::unregister_light( light_ptr_t _light )
{
    auto remove_light_data = [ & ]< class Ty >( uint32_t _data_index, sk::Graphics::cStructured_Buffer< Ty >& _buffer, std::vector< size_t >& _indices )
    {
        if( _data_index != _buffer.Size() - 1 )
        {
            auto& replaced_by = m_lights_[ _indices.back() ];
            auto& back        = _buffer.Back();
            
            replaced_by->m_data_index_ = _data_index;
            std::swap( _buffer[ _data_index ], back );
            _indices[ _data_index ] = replaced_by->m_registered_index_;
        }
        
        _buffer.PopBack();
        _indices.pop_back();
    };

    switch( _light->GetType() )
    {
    case Light::eType::kDirectional:
        remove_light_data( _light->m_data_index_, m_directional_buffer_, m_directional_light_indices_ );
    break;
    case Light::eType::kPoint:
        remove_light_data( _light->m_data_index_, m_point_buffer_, m_point_light_indices_ );
    break;
    case Light::eType::kSpot:
        remove_light_data( _light->m_data_index_, m_spot_buffer_, m_spot_light_indices_ );
    break;
    }
    
    _light->m_data_index_ = std::numeric_limits< uint32_t >::max();
    
    if( const auto index = _light->m_registered_index_; index != m_lights_.size() - 1 )
    {
        auto& back = m_lights_.back();
        
        back->m_registered_index_ = index;

        switch( back->GetType() ) {
        case Light::eType::kDirectional:
            m_directional_light_indices_[ back->m_data_index_ ] = index;
            break;
        case Light::eType::kPoint:
            m_point_light_indices_[ back->m_data_index_ ] = index;
            break;
        case Light::eType::kSpot:
            m_spot_light_indices_[ back->m_data_index_ ] = index;
            break;
        }
        
        std::swap( m_lights_[ index ], back );
    }
    
    m_lights_.pop_back();
    
    if( _light->GetSettings().casts_shadows )
        remove_shadow_caster( _light );
}

void sk::Scene::cLight_Manager::add_shadow_caster( const light_ptr_t& _light )
{
    if( _light->m_settings_.type == Light::eType::kPoint )
    {
        SK_WARNING( sk::Severity::kEngine, "Warning: Point lights can't be shadow casters!" );
        _light->m_settings_.casts_shadows = false;
        return;
    }

    _light->m_shadow_data_index_ = static_cast< uint32_t >( m_shadow_casters_.size() );
    m_shadow_casters_.emplace_back( _light );

    if( const auto settings = _light->GetSettings(); !Memory::is_power_of_two( settings.shadow_resolution ) )
    {
        SK_BREAK;
        _light->m_settings_.shadow_resolution = Math::roundToPow2( settings.shadow_resolution );
    }
    
    m_shadow_caster_buffer_.EmplaceBack( Light::sShadowCaster{
        .atlas_start  = { 0, 0 },
        .atlas_end    = { 0, 0 },
        .light_view_proj = _light->GetViewProjMatrix()
    } );
    
    m_preserve_atlas_ = false;
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

    m_preserve_atlas_ = false;
}

void sk::Scene::cLight_Manager::mark_buffer_dirty( const Light::eType _type )
{
    switch( _type )
    {
    case Light::eType::kDirectional:
        m_directional_buffer_.MarkDirty();
        break;
    case Light::eType::kPoint:
        m_point_buffer_.MarkDirty();
        break;
    case Light::eType::kSpot:
        m_spot_buffer_.MarkDirty();
        break;
    }
}

void sk::Scene::cLight_Manager::mark_shadow_buffer_dirty( const bool _preserve_atlas )
{
    m_preserve_atlas_ &= _preserve_atlas;
    m_shadow_caster_buffer_.MarkDirty();
}

void sk::Scene::cLight_Manager::compute_atlas()
{
    m_preserve_atlas_ = true;
    // From: https://lisyarus.github.io/blog/posts/texture-packing.html

    const auto count = m_shadow_casters_.size();

    // First: Resolution, Second: Shadow Caster Index
    using shadow_size_vec_t = std::vector< std::pair< uint32_t, uint32_t > >;

    shadow_size_vec_t sorted_sizes( count );

    for( size_t i = 0; i < sorted_sizes.size(); ++i )
        sorted_sizes[ i ] = std::make_pair( m_shadow_casters_[ i ]->m_settings_.shadow_resolution, i );

    std::ranges::sort( sorted_sizes, []( const auto& _a, const auto& _b ){ return _a.first > _b.first; } );

    // TODO: Figure out a better way to guess the atlas width.
    // Or find an algorithm that takes it into consideration.
    // Or decide a constant max width.
    const auto largest_caster    = sorted_sizes.front().first;
    const auto atlas_width_guess = largest_caster * 2;

    std::vector< cVector2u32 > ladder;

    cVector2u32 pen{ 0, 0 };
    m_computed_atlas_size_.x = atlas_width_guess;
    m_computed_atlas_size_.y = 0;

    for( auto& [ resolution, caster_index ] : sorted_sizes )
    {
        auto& data = m_shadow_caster_buffer_[ caster_index ];
        data.atlas_start = { pen.x, pen.y };
        data.atlas_end   = { pen.x + resolution, pen.y + resolution };

        m_computed_atlas_size_.y = std::max( m_computed_atlas_size_.y, pen.y + resolution );

        pen.x += resolution;

        if( !ladder.empty() && ladder.back().y == pen.y + resolution )
            ladder.back().x = pen.x;
        else
            ladder.emplace_back( pen.x, pen.y + resolution );

        if( pen.x == atlas_width_guess )
        {
            ladder.pop_back();

            pen.y += resolution;
            if( !ladder.empty() )
                pen.x = ladder.back().x;
            else
                pen.x = 0;
        }
    }

    m_computed_atlas_size_.y = sk::Math::ceilTo< uint32_t >( static_cast< float >( m_computed_atlas_size_.y ) / largest_caster ) * largest_caster;
}
