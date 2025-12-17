/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <set>
#include <Assets/Asset.h>

#include <glbinding/gl/types.h>
#include <glbinding/gl/enum.h>

namespace sk::Shader
{
    enum class eType : uint8_t
    {
        kVertex,
        kFragment,
        // TODO: Review how and if it could be implemented.
        kGeometry,
        // TODO: Compute shader implementation.
        kCompute,
    };
} // sk::Shader

namespace sk::Assets
{
    SK_ASSET_CLASS( Shader )
    {
        SK_CLASS_BODY( Shader )
    public:
        using eType = sk::Shader::eType;
        
        class cShader_Reflection
        {
        public:
            enum eFlags : uint32_t
            {
                kNone   = 0,
                
                kHidden  = 0x01,
                kPartial = 0x02,
            };
            
            struct sAttribute
            {
                cStringID   name;
                type_info_t type;
                
                uint32_t   index;
                uint32_t   stride;
            };

            struct sUniform
            {
                cStringID  name;
                
                gl::GLenum type;
                // Either the index or bytewise offset.
                uint32_t   location;
                size_t     size;
            };
            
            struct sBlock
            {
                using uniform_map_t = std::unordered_map< str_hash, sUniform >;
                
                cStringID     name;
                uniform_map_t uniforms;
                size_t        size;
            };

            struct sTexture
            {
                uint32_t location;
            };

            struct sSampler
            {
                uint32_t location;
            };
            
            explicit cShader_Reflection( gl::GLuint _program );
            
            [[ nodiscard ]] auto& GetAttributes() const { return m_attributes_; }
            [[ nodiscard ]] auto& GetBlocks    () const { return m_blocks_; }
            [[ nodiscard ]] auto& GetUniforms  () const { return m_global_uniforms_; }
            
        private:
            void init();
            
            void get_attributes();
            void get_uniforms();
            void get_blocks();
            
            sUniform get_uniform( gl::GLuint _index ) const;
            
            void cleanup();
            
            // For the reflection only.
            gl::GLuint m_program_;
            uint32_t   m_max_attribute_name_size_;
            uint32_t   m_max_block_name_size_;
            uint32_t   m_max_uniform_name_size_;
            
            gl::GLchar*  m_uniform_name_buffer_;      
            
            std::set< gl::GLuint > m_locked_uniforms_;
            
            std::vector< sAttribute > m_attributes_;
            
            unordered_map< str_hash, sUniform >   m_global_uniforms_;
            unordered_map< str_hash, sBlock >     m_blocks_;
            
            unordered_map< str_hash, sTexture >   m_textures_;
            unordered_map< str_hash, sSampler >   m_samplers_;
        };

        cShader( eType _type, const void* _buffer, size_t _size );
        ~cShader() override;

        // TODO: Create a shader group class to replace the shaders themselves being linked.
        // This will reduce the confusion and increased number of cases the current system creates.
        
        // Binds another shader to this shader.
        cShader& LinkShader( const cShader& _other_shader );
        // Binds another shader to this shader.
        cShader& LinkShader( eType _type, const cShader& _other_shader );
        
        void UnlinkShader( eType _type );

        // Internal
        [[ nodiscard ]] auto get_shader() const { return m_shader_; }

    private:
        void link_shaders() const;

        cShared_ptr< cShader_Reflection > m_reflection_;
        
        cWeak_Ptr< cShader > m_vertex_shader_   = nullptr;
        cWeak_Ptr< cShader > m_fragment_shader_ = nullptr;

        gl::GLenum m_type_;
        gl::GLuint m_shader_;
        gl::GLuint m_program_;
    };
} // sk::Assets::

DECLARE_CLASS( sk::Assets::Shader )