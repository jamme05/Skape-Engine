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

    sk_public:

        explicit cMesh( const std::string& _name );
        ~cMesh() override;

        void Save() override {}

        auto& GetIndexBuffer  ()       { return m_indices_; }
        auto& GetIndexBuffer  () const { return m_indices_; }
        auto& GetVertexBuffers()       { return m_vertex_buffers_; }
        auto& GetVertexBuffers() const { return m_vertex_buffers_; }

    sk_private:
        using buffer_t     = Graphics::cDynamic_Buffer*;
        using buffer_map_t = unordered_map< str_hash, buffer_t >;

        buffer_t     m_indices_;
        buffer_map_t m_vertex_buffers_;
    };
} // sk::Assets

REGISTER_CLASS( sk::Assets::Mesh )