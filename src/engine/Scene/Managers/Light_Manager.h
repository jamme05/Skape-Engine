#pragma once

#include <Graphics/Buffer/Buffer.h>
#include <Math/Vector3.h>
#include <Misc/Singleton.h>
#include <Misc/UUID.h>
#include <Scene/Components/LightComponent.h>

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
            static constexpr auto kConstantPointMax       = 4;
            static constexpr auto kConstantSpotMax        = 4;
            
            uint32_t directional_light_count;
            uint32_t point_light_count;
            uint32_t spot_light_count;
            // Flags: 0x01 = Directional extended, 0x02 = Point extended, 0x04 = Spot extended.
            // Extended means to use a structured buffer instead of the faster uniform buffer
            uint32_t uses_extended;
            // The user should at MOST have two directional lights, so we keep that as the safe range.
            Light::sDirectionalLight directional_light[ kConstantDirectionalMax ];
            Light::sPointLight       point_light      [ kConstantPointMax ];
            Light::sSpotLight        spot_light       [ kConstantSpotMax ];
            // TODO: Decide how many other lights we should have.
        };
        
        cLight_Manager();
        
        void Update();

        [[ nodiscard ]] auto  GetLights       () const -> const light_vec_t&;
        [[ nodiscard ]] auto  GetShadowCasters() const -> const light_vec_t&;
        
        [[ nodiscard ]] auto& GetLightBuffer       () const { return m_light_settings_buffer_; }
        [[ nodiscard ]] auto& GetDirectionalBuffer () const { return m_directional_buffer_;    }
        [[ nodiscard ]] auto& GetPointBuffer       () const { return m_point_buffer_;          }
        [[ nodiscard ]] auto& GetSpotBuffer        () const { return m_spot_buffer_;           }
        [[ nodiscard ]] auto& GetShadowCasterBuffer() const { return m_shadow_caster_buffer_;  }
    private:
        void register_light  ( light_ptr_t _light );
        void update_light    ( light_ptr_t _light, const Light::sSettings& _previous_settings );
        void unregister_light( light_ptr_t _light );
        
        void add_shadow_caster   ( const light_ptr_t& _light );
        void remove_shadow_caster( const light_ptr_t& _light );
        
        void mark_buffer_dirty( Light::eType _type );
        void mark_shadow_buffer_dirty();
        
        void compute_atlas();
        
        using settings_buffer_t    = Graphics::cConstant_Buffer< sLightSettings >;
        using directional_buffer_t = Graphics::cStructured_Buffer< Light::sDirectionalLight >;
        using point_buffer_t       = Graphics::cStructured_Buffer< Light::sPointLight >;
        using spot_buffer_t        = Graphics::cStructured_Buffer< Light::sSpotLight >;
        using shadow_buffer_t      = Graphics::cStructured_Buffer< Light::sShadowCaster >;
        
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
        
    };
} // sk::Scene::
