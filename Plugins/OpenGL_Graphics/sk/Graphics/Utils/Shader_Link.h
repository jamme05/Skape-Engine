

#pragma once

#include <sk/Assets/Shader.h>
#include <sk/Assets/Access/Asset_Ref.h>

namespace sk::Assets
{
    class cMaterial;
}

namespace sk::Graphics::Utils
{
    class cShader_Link
    {
    public:
        cShader_Link() = default;
        cShader_Link( const cShared_ptr< cAsset_Meta >& _vertex_shader, const cShared_ptr< cAsset_Meta >& _fragment_shader );
        cShader_Link( const cShader_Link& _other );
        cShader_Link( cShader_Link&& _other ) noexcept;
        ~cShader_Link();
        
        cShader_Link& operator=( const cShader_Link& _other );
        cShader_Link& operator=( cShader_Link&& _other ) noexcept;
        
        bool HasUpdated() const;
        bool IsValid() const;
        bool IsReady() const;
        
        void Complete();
        
        auto GetVertexShader  () const -> const Assets::cShader&;
        auto GetFragmentShader() const -> const Assets::cShader&;
        
        auto GetReflection() const -> cShared_ptr< cShader_Reflection >;
        
        void Use() const;
        
        auto get_program() const { return m_program_; }
        
    private:
        void on_shader_changed( Assets::eEventType _event, cAsset_Ref< Assets::cShader >& );
        void link_shaders();
        void unlink_shaders();
        
        gl::GLuint m_program_     = 0;
        bool       m_is_linked_   = false;
        bool       m_has_updated_ = false;
        
        cShared_ptr< cShader_Reflection > m_reflection_;
        
        cAsset_Ref< Assets::cShader > m_vertex_shader_;
        cAsset_Ref< Assets::cShader > m_fragment_shader_;
    };
} // sk::Graphics::Utils
