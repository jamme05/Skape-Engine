/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>
#include <Containers/Map.h>

namespace sk::Graphics
{
    class cDynamic_Buffer;
} // sk::Graphics::

// TODO: Decide if I should move the Mesh from the opengl module to the main engine.
namespace sk::Assets
{
    SK_ASSET_CLASS( Mesh )
    {
        SK_CLASS_BODY( Mesh )

        using buffer_t     = Graphics::cDynamic_Buffer*;
        using buffer_map_t = unordered_map< str_hash, buffer_t >;

    sk_public:

        explicit cMesh( const std::string& _name );
        ~cMesh() override;

        void Save  () override {}
        void Load  () override {}
        void Unload() override {}

        [[ nodiscard ]] auto GetIndexBuffer()       { return m_indices_; }
        [[ nodiscard ]] auto GetIndexBuffer() const { return m_indices_; }
        SK_FUNCTION_P( GetIndexBuffer )

        [[ nodiscard ]] auto& GetVertexBuffers()       { return m_vertex_buffers_; }
        [[ nodiscard ]] auto& GetVertexBuffers() const { return m_vertex_buffers_; }
        SK_FUNCTION_P( GetVertexBuffers )

    sk_private:
        buffer_t     m_indices_;
        buffer_map_t m_vertex_buffers_;
    };
} // sk::Assets

DECLARE_CLASS( sk::Assets::Mesh )
