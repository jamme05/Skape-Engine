

#pragma once

#include "Component.h"
#include "Assets/Utils/Asset_List.h"

namespace sk::Scene
{
    class cLight_Manager;
} // sk::Scene

namespace sk::Scene::Light
{
    enum class eType : uint8_t
    {
        kDirectional,
        kPoint,
        kSpot,
    };

    struct sSettings
    {
        eType type          = eType::kDirectional;
        bool  casts_shadows = true;
            
        // Directional
        float intensity = 1.0f;
            
        // Point/Spot
        float range = 10.0f;
            
        // Spot
        float inner_angle = 45.0f;
        float outer_angle = 60.0f;
    };
    
    struct alignas( 16 ) sDirectionalLight
    {
        // color = light.color * light.intensity
        cVector4f color;
        cVector3f direction;
        // The index that contains this lights shadow casting info
        uint32_t  shadow_cast_index;
    };
    
    struct alignas( 16 ) sPointLight
    {
        // color = light.color * light.intensity
        cVector3f color;
        cVector3f position;
        float     radius;
        // The index that contains this lights shadow casting info
        uint32_t  shadow_cast_index;
    };
    
    struct alignas( 16 ) sSpotLight
    {
        // color = light.color * light.intensity
        cVector3f color;
        // Length of direction is the radius.
        cVector3f direction;
        cVector3f position;
        // The index that contains this lights shadow casting info
        float     inner_angle;
        float     outer_angle;
        uint32_t  shadow_cast_index;
    };
} // sk::Scene::Light::

namespace sk::Object::Components
{
    SK_COMPONENT_CLASS( LightComponent )
    {
        SK_CLASS_BODY( LightComponent )
        
        friend class sk::Scene::cLight_Manager;
    public:
        using type_t     = sk::Scene::Light::eType;
        using settings_t = sk::Scene::Light::sSettings;
        
        explicit cLightComponent( const settings_t& _settings = {} );
        ~cLightComponent() override;
        
        void update() override;

        [[ nodiscard ]]
        auto GetSettings() const -> const settings_t&;
        void SetSettings( const settings_t& _settings );
        
        // TODO: Add more functions to get/set values inside of the settings.
        [[ nodiscard ]]
        auto GetType() const -> type_t;
        void SetType( type_t _type );

        [[ nodiscard ]] auto GetViewProjMatrix() const -> const cMatrix4x4f&;
    private:
        settings_t  m_settings_;
        uint32_t    m_shadow_data_index_ = std::numeric_limits< uint32_t >::max();
        size_t      m_data_index_        = std::numeric_limits< size_t >::max();
        cMatrix4x4f m_view_proj_matrix_;
        
    };
} // sk::Object::Components

DECLARE_CLASS( sk::Object::Components::LightComponent )