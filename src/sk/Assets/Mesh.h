/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Asset.h>
#include <sk/Containers/Map.h>

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
        enum class eIndexType : uint8_t
        {
            k16,
            k32
        };

        // TODO: Primitives
        struct sPrimitive
        {
            size_t offset;
            size_t count;
        };
        
        using buffer_t     = cShared_ptr< Graphics::cDynamic_Buffer >;
        using buffer_map_t = unordered_map< cStringID, buffer_t >;

        cMesh( const std::string& _name );
        ~cMesh() override;

        void CreateIndexBufferFrom( eIndexType _type, const void* _data, size_t _item_count );

        [[ nodiscard ]] auto& GetName() const { return m_name_; }
        
        [[ nodiscard ]] auto& GetIndexBuffer()       { return m_indices_; }
        [[ nodiscard ]] auto& GetIndexBuffer() const { return m_indices_; }

        [[ nodiscard ]] auto& GetVertexBuffers()       { return m_vertex_buffers_; }
        [[ nodiscard ]] auto& GetVertexBuffers() const { return m_vertex_buffers_; }

        [[ nodiscard ]] bool  IsValid() const;
        
    private:
        std::string  m_name_;
        buffer_t     m_indices_;
        buffer_map_t m_vertex_buffers_;
    };
} // sk::Assets

SK_DECLARE_CLASS( sk::Assets::Mesh )