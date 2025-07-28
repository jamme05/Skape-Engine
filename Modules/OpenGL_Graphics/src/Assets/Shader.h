/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <glbinding/gl/types.h>

#include <Assets/Asset.h>
#include <glbinding/gl/enum.h>

namespace sk::Shader
{
    enum class eType : uint8_t
    {
        kVertex,
        kFragment,
        // TODO: Review how it could be implamented.
        kGeometry,
    };
} // sk::Shader

namespace sk::Assets
{
    SK_ASSET_CLASS( Shader )
    {
    public:
        using eType = sk::Shader::eType;

        cShader( const std::string& _name, eType _type, const void* _buffer, size_t _size );
        ~cShader() override;

        // Binds another shader to this shader.
        cShader& LinkShader( const cShader& _other_shader );
        // Binds another shader to this shader.
        cShader& LinkShader( eType _type, const cShader& _other_shader );

        // Internal
        auto get_shader() const { return m_shader_; }

        void Save  () override {}
        void Load  () override {}
        void Unload() override {}

    private:
        void link_shaders();

        cWeak_Ptr< cShader > m_vertex_shader_   = nullptr;
        cWeak_Ptr< cShader > m_fragment_shader_ = nullptr;

        gl::GLenum m_type_;
        gl::GLuint m_shader_;
        gl::GLuint m_program_;
    };
} // sk::Assets::
