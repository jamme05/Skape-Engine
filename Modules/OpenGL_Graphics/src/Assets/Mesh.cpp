/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Mesh.h"

#include <Graphics/Buffer/Dynamic_Buffer.h>

#include <ranges>

namespace sk::Assets
{
    cMesh::cMesh( const std::string& _name )
    : cAsset( _name )
    , m_indices_( SK_SINGLE( Graphics::cDynamic_Buffer,  ) )
    {} // cMesh

    cMesh::~cMesh()
    {
        SK_FREE( m_indices_ );

        for( const auto& buffer : m_vertex_buffers_ | std::views::values )
            SK_FREE( buffer );
    }
} // sk::Assets::