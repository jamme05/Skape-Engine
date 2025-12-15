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

    public:
        using buffer_t     = cShared_ptr< Graphics::cDynamic_Buffer >;
        using buffer_map_t = unordered_map< str_hash, buffer_t >;

        cMesh();
        ~cMesh() override;

        [[ nodiscard ]] auto& GetIndexBuffer()       { return m_indices_; }
        [[ nodiscard ]] auto& GetIndexBuffer() const { return m_indices_; }

        [[ nodiscard ]] auto& GetVertexBuffers()       { return m_vertex_buffers_; }
        [[ nodiscard ]] auto& GetVertexBuffers() const { return m_vertex_buffers_; }

        [[ nodiscard ]] bool  IsValid() const;
        
    private:
        buffer_t     m_indices_;
        buffer_map_t m_vertex_buffers_;
    };
} // sk::Assets

DECLARE_CLASS( sk::Assets::Mesh )