/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Mesh.h"

#include <sk/Graphics/Buffer/Dynamic_Buffer.h>

namespace sk::Assets
{
    cMesh::cMesh( const std::string& _name )
    : m_indices_( sk::MakeShared< Graphics::cDynamic_Buffer >( _name + ": Indices", Graphics::Buffer::eType::kIndex, false ) )
    {} // cMesh

    cMesh::~cMesh()
    {
        m_indices_ = nullptr;
        m_vertex_buffers_.clear();
    }

    void cMesh::CreateIndexBufferFrom( const eIndexType _type, const void* _data, const size_t _item_count )
    {
        switch( _type )
        {
        case eIndexType::k16: m_indices_->AlignAs< uint16_t >(); break;
        case eIndexType::k32: m_indices_->AlignAs< uint32_t >(); break;
        }
        
        m_indices_->Resize( _item_count );

        if( _data != nullptr )
            memcpy( m_indices_->RawData(), _data, _item_count * m_indices_->GetItemSize() );
    }

    bool cMesh::IsValid() const
    {
        if( !m_indices_->IsValid() )
            return false;

        return std::ranges::none_of( m_vertex_buffers_, []( const auto& _buffer )
        {
            return _buffer.second->IsValid();
        } );
    }
} // sk::Assets::