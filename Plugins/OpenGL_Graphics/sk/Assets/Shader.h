/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Asset.h>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/types.h>

namespace sk::Graphics::Utils
{
    class cShader_Link;
    class cShader_Reflection;
} // sk::Graphics::Utils::

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
        
        friend class sk::Graphics::Utils::cShader_Link;
        
    public:
        using eType        = sk::Shader::eType;
        using reflection_t = sk::Graphics::Utils::cShader_Reflection;
        
        cShader( eType _type, const void* _buffer, size_t _size );
        ~cShader() override;

        // TODO: Create a shader group class to replace the shaders themselves being linked.
        // This will reduce the confusion and increased number of cases the current system creates.

        // INTERNAL
        auto get_shader_object() const { return m_shader_; }

    private:
        cShared_ptr< reflection_t > m_reflection_;
        
        cWeak_Ptr< cShader > m_vertex_shader_   = nullptr;
        cWeak_Ptr< cShader > m_fragment_shader_ = nullptr;

        gl::GLenum m_type_;
        gl::GLuint m_shader_;
    };
} // sk::Assets::

DECLARE_CLASS( sk::Assets::Shader )