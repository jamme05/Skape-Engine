#pragma once

#include <sk/Graphics/Buffer/Buffer.h>
#include <sk/Math/Vector3.h>
#include <sk/Misc/Singleton.h>
#include <sk/Scene/Components/LightComponent.h>

namespace sk::Graphics::Rendering
{
    class cRender_Target;
}

namespace sk::Scene
{
    class cLight_Manager : public cSingleton< cLight_Manager >
    {
        friend class sk::Object::Components::cLightComponent;
    public:
        using light_ptr_t = cWeak_Ptr< Object::Components::cLightComponent >;
        using light_vec_t = std::vector< light_ptr_t >;

        struct alignas( 16 ) sLightSettings
        {
            static constexpr auto kConstantDirectionalMax = 2;
            static constexpr auto kConstantPointMax       = 3;
            static constexpr auto kConstantSpotMax        = 3;
            
            uint32_t directional_light_count;
            uint32_t point_light_count;
            uint32_t spot_light_count;
            // Flags: 0x01 = Directional extended, 0x02 = Point extended, 0x04 = Spot extended.
            // Extended means to use a structured buffer instead of the faster uniform buffer
            uint32_t uses_extended;
            // The user should at MOST have two directional lights, so we keep that as the safe range.
            Light::sDirectionalLight constant_directional_light[ kConstantDirectionalMax ];
            Light::sPointLight       constant_point_light      [ kConstantPointMax ];
            Light::sSpotLight        constant_spot_light       [ kConstantSpotMax ];
            // TODO: Decide how many other lights we should have.
            cVector2u32 atlas_size;
            // TODO: Fix padding.
        };
        
        cLight_Manager();
        
        void Update();

        [[ nodiscard ]] auto  GetAtlasSize    () const -> cVector2u32;

        [[ nodiscard ]] auto  GetLights       () const -> const light_vec_t&;
        [[ nodiscard ]] auto  GetShadowCasters() const -> const light_vec_t&;
        
        [[ nodiscard ]] auto& GetLightBuffer       () const { return m_light_settings_buffer_; }
        [[ nodiscard ]] auto& GetDirectionalBuffer () const { return m_directional_buffer_;    }
        [[ nodiscard ]] auto& GetPointBuffer       () const { return m_point_buffer_;          }
        [[ nodiscard ]] auto& GetSpotBuffer        () const { return m_spot_buffer_;           }
        [[ nodiscard ]] auto& GetShadowCasterBuffer() const { return m_shadow_caster_buffer_; }
    private:
        // TODO: Use light references instead of weak ptr.
        void register_light  ( light_ptr_t _light );
        void update_light    ( light_ptr_t _light, const Light::sSettings& _previous_settings );
        void unregister_light( light_ptr_t _light );
        
        void add_shadow_caster   ( const light_ptr_t& _light );
        void remove_shadow_caster( const light_ptr_t& _light );
        
        void mark_buffer_dirty( Light::eType _type );
        void mark_shadow_buffer_dirty( bool _preserve_atlas );
        
        void compute_atlas();
        
        using settings_buffer_t    = Graphics::cConstant_Buffer< sLightSettings >;
        using directional_buffer_t = Graphics::cStructured_Buffer< Light::sDirectionalLight >;
        using point_buffer_t       = Graphics::cStructured_Buffer< Light::sPointLight >;
        using spot_buffer_t        = Graphics::cStructured_Buffer< Light::sSpotLight >;
        using shadow_buffer_t      = Graphics::cStructured_Buffer< Light::sShadowCaster >;
        using render_target_t      = cShared_ptr< Graphics::Rendering::cRender_Target >;

        light_vec_t m_lights_;
        light_vec_t m_shadow_casters_;
        
        settings_buffer_t m_light_settings_buffer_;
        shadow_buffer_t   m_shadow_caster_buffer_;
        
        directional_buffer_t m_directional_buffer_;
        point_buffer_t       m_point_buffer_;
        spot_buffer_t        m_spot_buffer_;
        
        std::vector< size_t > m_directional_light_indices_;
        std::vector< size_t > m_point_light_indices_;
        std::vector< size_t > m_spot_light_indices_;

        cVector2u32 m_computed_atlas_size_;
        uint32_t    m_atlas_size_;
        // If we should preserve the atlas between updates.
        bool        m_preserve_atlas_;
        // TODO: Remove padding
    };
} // sk::Scene::
