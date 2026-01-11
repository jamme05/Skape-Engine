

#pragma once

#include <variant>

#include <Scene/Components/Component.h>

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
        
        cColor color = Color::kWhite;
            
        // Directional
        float intensity = 1.0f;
            
        // Point/Spot
        float radius = 10.0f;
            
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
        int32_t   shadow_cast_index;
    };
    
    struct alignas( 16 ) sPointLight
    {
        // color = light.color * light.intensity
        cVector3f color;
        float     radius;
        cVector3f position;
        // The index that contains this lights shadow casting info
        int32_t   shadow_cast_index;
    };
    
    struct alignas( 16 ) sSpotLight
    {
        // color = light.color * light.intensity
        cVector3f color;
        // direction = world_position * radius
        cVector3f direction;
        cVector3f position;
        float     inner_angle;
        float     outer_angle;
        // The index that contains this lights shadow casting info
        int32_t   shadow_cast_index;
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
        using data_t     = std::variant< Scene::Light::sDirectionalLight, Scene::Light::sPointLight, Scene::Light::sSpotLight >;
        
        explicit cLightComponent( const settings_t& _settings = {} );
        ~cLightComponent() override;
        
        void update() override;
        
        // TODO: Add more functions to get/set values inside of the settings.
        [[ nodiscard ]]
        auto GetSettings() const -> const settings_t&;
        [[ nodiscard ]]
        auto GetType() const -> type_t;
        
        void SetSettings( const settings_t& _settings );
        void SetType( type_t _type );
        

        [[ nodiscard ]]
        auto GetViewProjMatrix() const -> const cMatrix4x4f&;
        [[ nodiscard ]]
        auto GetData() const -> const data_t&;
        
    private:
        void fix_data();
        
        void fix_directional_data();
        void fix_point_data();
        void fix_spot_data();
        
        void update_data();
        
        data_t      m_data_;
        settings_t  m_settings_;
        
        uint32_t    m_registered_index_  = std::numeric_limits< uint32_t >::max();
        uint32_t    m_shadow_data_index_ = std::numeric_limits< uint32_t >::max();
        uint32_t    m_data_index_        = std::numeric_limits< uint32_t >::max();
        
        cMatrix4x4f m_view_proj_matrix_;
        
    };
} // sk::Object::Components

DECLARE_CLASS( sk::Object::Components::LightComponent )