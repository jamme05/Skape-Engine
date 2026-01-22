//
//
// COPYRIGHT William Ask S. Ness 2025
//
//

#pragma once

#include <sk/Assets/Asset.h>
#include <sk/Assets/Shader.h>
#include <sk/Assets/Texture.h>
#include <sk/Assets/Access/Asset_Ref.h>
#include <sk/Graphics/Buffer/Dynamic_Buffer.h>
#include <sk/Graphics/Utils/Shader_Link.h>
#include <sk/Math/Matrix4x4.h>
#include <sk/Math/Vector4.h>

#include <variant>


namespace sk::Graphics::Rendering
{
    class cRender_Target;
}

namespace sk::Graphics::Utils
{
    struct sSampler;
    struct sUniform;
    struct sBlock;
    class cShader_Reflection;
} // sk::Graphics::Utils::

namespace sk
{
    namespace Graphics::Rendering
    {
        class cFrame_Buffer;
    }

    // TODO: Move this to a different file
    template< class Ty, class... Types >
    constexpr bool kOneOf = ( std::is_same_v< Ty, Types > ||... );
    template< class Ty >
    constexpr bool kOneOf< Ty > = true;
} // sk::

namespace sk::Assets
{
    SK_ASSET_CLASS( Material )
    {
        SK_CLASS_BODY( Material )
        
        friend class sk::Graphics::Utils::cShader_Link;
        friend class sk::Graphics::Rendering::cFrame_Buffer;
    public:
        class cBlock
        {
            friend class cMaterial;
            friend class sk::Graphics::Rendering::cFrame_Buffer;
            friend class sk::Graphics::Utils::cShader_Link;
        public:
            using variant_t = std::variant<
                int32_t,  cVector2i32, cVector3i32, cVector4i32,
                uint32_t, cVector2u32, cVector3u32, cVector4u32,
                float, cVector2f, cVector3f, cVector4f, cMatrix4x4f
            >;
            using direct_variant_t = std::variant<
                int32_t*,  cVector2i32*, cVector3i32*, cVector4i32*,
                uint32_t*, cVector2u32*, cVector3u32*, cVector4u32*,
                float*, cVector2f*, cVector3f*, cVector4f*, cMatrix4x4f*
            >;
            template< class Ty >
            static constexpr bool kValidType = kOneOf< Ty, int32_t, uint32_t, cVector4i32, cVector4u32, float, cVector4f, cMatrix4x4f >;

            struct sUniform
            {
                // TODO: Support uniforms with array values.
                
                using uniform_t = Graphics::Utils::sUniform;
                
                std::string_view pretty_name;
                direct_variant_t accessor;
                uint32_t    byte_size;
                uint32_t    array_size;
                // Actual vector size.
                uint32_t    vector_size;
                uint32_t    matrix_size;
                uniform_t*  info;
            };
            
            using buffer_t      = Graphics::cUnsafe_Buffer;
            using block_t       = Graphics::Utils::sBlock;
            using uniform_map_t = std::unordered_map< str_hash, sUniform >;
            using uniform_vec_t = vector< sUniform* >;
            
            cBlock() = default;
            cBlock( const cMaterial& _owner, std::string _name, const block_t* _info, size_t _binding );

            bool SetUniform( const cStringID& _name, const auto& _value );
            bool SetUniformRaw( const cStringID& _name, const void* _data, size_t _size );
            
            auto& GetUniformMap() const { return m_uniform_map_; }
            auto& GetUniformVec() const { return m_uniform_vec_; }
            
        private:
            std::string    m_pretty_name_;
            const block_t* m_info_;
            size_t         m_binding_;
            buffer_t       m_buffer_;
            
            uniform_map_t m_uniform_map_;
            uniform_vec_t m_uniform_vec_;
            
            const cMaterial* m_owner_;
        };
        struct sInvalid{};

        enum class eDepthTest : uint8_t
        {
            kNever,
            kAlways,
            kLess,
            kLessEqual,
            kGreater,
            kGreaterEqual,
            kEqual,
            kNotEqual,
        };
        
        explicit cMaterial( Graphics::Utils::cShader_Link&& _shader_link );
        
        void Complete();
        
        auto  GetBlock( const cStringID& _name ) -> cBlock*;
        auto& GetBlocks() const { return m_block_map_; }

        void  SetTexture( const cStringID& _name, std::nullptr_t );
        void  SetTexture( const cStringID& _name, const cShared_ptr< Graphics::Rendering::cRender_Target >& _texture );
        void  SetTexture( const cStringID& _name, const cShared_ptr< cAsset_Meta >& _texture_meta );
        auto& GetTextures() const { return m_textures_; }
        
        void  SetDepthTest( eDepthTest _depth_test );
        auto  GetDepthTest() const -> eDepthTest;
        
        auto GetShaderLink() const -> const Graphics::Utils::cShader_Link&;
        
        bool IsReady() const;
        
        void Update();
        
    private:
        struct sTexture
        {
            using sampler_t = const Graphics::Utils::sSampler*;
            using variant_t = std::variant< sInvalid, cAsset_Ref< cTexture >, cShared_ptr< Graphics::Rendering::cRender_Target > >;
            size_t    index;
            sampler_t sampler;
            variant_t texture;
        };
        
        using block_map_t       = unordered_map< str_hash, cBlock >;
        using block_vec_t       = vector< cBlock* >;
        using reflection_t      = cShared_ptr< Graphics::Utils::cShader_Reflection >;
        using shader_ptr_t      = cAsset_Ref< cShader, eAsset_Ref_Mode::kManual >;
        using sampler_map_t     = unordered_map< str_hash, sTexture* >;
        using texture_vec_t     = std::vector< sTexture >;
        using texture_ref_vec_t = std::vector< cAsset_Ref< cTexture > >;
        
        void create_data();
        
        block_map_t   m_block_map_;
        block_vec_t   m_block_vec_;
        sampler_map_t m_sampler_map_;
        texture_vec_t m_textures_;

        texture_ref_vec_t m_texture_refs_;
        
        Graphics::Utils::cShader_Link m_shader_link_;
        
        // TODO: Add more settings.
        eDepthTest m_depth_test_ = eDepthTest::kLessEqual;
    };
    
    bool cMaterial::cBlock::SetUniform( const cStringID& _name, const auto& _value )
    {
        using value_type   = std::remove_cvref_t< decltype( _value ) >;
        using element_type = std::remove_all_extents_t< value_type >;
    
        constexpr auto array_size = sizeof( value_type ) / sizeof( element_type );
    
        const auto itr = m_uniform_map_.find( _name );
    
        SK_BREAK_RET_IF( sk::Severity::kEngine, itr == m_uniform_map_.end(),
            TEXT( "Warning: Unable to find uniform with the name, {}", _name.view() ), false )
    
        auto& uniform = itr->second;
    
        size_t elements = 1;
    
        if constexpr( std::is_array_v< value_type > )
        {
            if( array_size > uniform.array_size )
            {
                SK_BREAK;
                SK_WARNING( sk::Severity::kGraphics,
                    "Warning: Array size provided is greater than what the uniform supports. Clamping size." )
            
                elements = uniform.array_size;
            }
            else
                elements = array_size;
        }
    
        std::visit( [ & ]< class Ty >( Ty*& _uniform_ptr )
        {
            if constexpr( !std::is_convertible_v< element_type, Ty > )
            {
                SK_WARNING( sk::Severity::kGraphics, "Type is unsupported" )
            }
            else
            {
                if constexpr( std::is_array_v< value_type > )
                {
                    std::copy_n( std::addressof( _value ), elements, _uniform_ptr );
                }
                else
                {
                    // If you get an error here, that means you've provided an invalid type.
                    auto& value = *_uniform_ptr;
                    value = _value;
                }
            }
        
        }, uniform.accessor );
    
        return true;
    }
    
} // sk::Assets

DECLARE_CLASS( sk::Assets::Material )
