

#pragma once

#include <sk/Scene/Components/Component.h>

#include <variant>

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
        eType    type              = eType::kDirectional;
        bool     casts_shadows     = true;
        uint16_t shadow_resolution = 256;

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
        cVector4f color;
        cVector3f position;
        float     radius;
        // The index that contains this lights shadow casting info
        int32_t   shadow_cast_indices[ 6 ];
    };
    
    struct alignas( 16 ) sSpotLight
    {
        // Ordered this way due to possible padding.
        // color = light.color * light.intensity
        cVector3f color;
        float     inner_angle;
        // direction = world_position * radius
        cVector3f direction;
        float     outer_angle;
        cVector3f position;
        // The index that contains this lights shadow casting info
        int32_t   shadow_cast_index;
    };
    
    struct alignas( 16 ) sShadowCaster
    {
        cVector2u32 atlas_start;
        cVector2u32 atlas_end;
        cMatrix4x4f light_view_proj;
    };
} // sk::Scene::Light::

namespace sk::Object::Components
{
    SK_COMPONENT_CLASS( LightComponent )
    {
        SK_CLASS_BODY( LightComponent )
        
        friend class sk::Scene::cLight_Manager;
    public:
        struct sInvalid{};
        
        using type_t     = sk::Scene::Light::eType;
        using settings_t = sk::Scene::Light::sSettings;
        using data_ptr_t = std::variant< sInvalid, Scene::Light::sDirectionalLight*, Scene::Light::sPointLight*, Scene::Light::sSpotLight* >;
        using data_t     = std::variant< Scene::Light::sDirectionalLight, Scene::Light::sPointLight, Scene::Light::sSpotLight >;
        using caster_t   = Scene::Light::sShadowCaster;
        
        explicit cLightComponent( const settings_t& _settings = {} );
        explicit cLightComponent( cSerializedObject& _object );
        ~cLightComponent() override;
        
        void update() override;

        void UpdateViewProjectionMatrix();

        // TODO: Allow layer selections.
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
        [[ nodiscard ]]
        auto GetShadowCasterData() const -> const caster_t*;

        auto Serialize() -> cSerializedObject override;
        
    private:
        void fix_data();
        
        void fix_directional_data( Scene::Light::sDirectionalLight& _data ) const;
        void fix_point_data( Scene::Light::sPointLight& _data ) const;
        void fix_spot_data( Scene::Light::sSpotLight& _data ) const;
        
        void update_data();
        void init_data();
        
        data_t     m_data_;

        settings_t m_settings_;
        
        uint32_t m_registered_index_  = std::numeric_limits< uint32_t >::max();
        uint32_t m_shadow_data_index_ = std::numeric_limits< uint32_t >::max();
        uint32_t m_data_index_        = std::numeric_limits< uint32_t >::max();
        
        cMatrix4x4f m_view_proj_matrix_;
    };
} // sk::Object::Components

SK_DECLARE_CLASS( sk::Object::Components::LightComponent )