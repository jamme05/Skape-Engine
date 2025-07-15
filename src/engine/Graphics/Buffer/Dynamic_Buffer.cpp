/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Dynamic_Buffer.h"

#include <chrono>

using namespace std;
// TYPE UNSAFE BUFFER

namespace sk::Graphics
{
    cDynamic_Buffer::cDynamic_Buffer()
    : m_byte_size_( 0 )
    , m_type_size_( 0 )
    , m_type_( nullptr )
    , m_buffer_( nullptr )
    {} // cDynamic_Buffer

    cDynamic_Buffer::cDynamic_Buffer( const std::string& _name, const Buffer::eType _type )
    : m_byte_size_( 0 )
    , m_type_size_( 0 )
    , m_type_( nullptr )
    , m_buffer_( SK_SINGLE( cUnsafe_Buffer, _name, 0, _type, false ) )
    {} // cDynamic_Buffer

    cDynamic_Buffer::cDynamic_Buffer( const cDynamic_Buffer& _other )
    : m_byte_size_( _other.m_byte_size_ )
    , m_type_size_( _other.m_type_size_ )
    , m_type_( _other.m_type_ )
    , m_buffer_{ SK_SINGLE( cUnsafe_Buffer, _other.m_buffer_->GetName(), m_byte_size_, false ) }
    {
        m_buffer_->Copy( *_other.m_buffer_ );
    }

    cDynamic_Buffer::cDynamic_Buffer( cDynamic_Buffer&& _other ) noexcept
    : m_byte_size_( _other.m_byte_size_ )
    , m_type_size_( _other.m_type_size_ )
    , m_type_( _other.m_type_ )
    , m_buffer_{ _other.m_buffer_ }
    {
        _other.m_buffer_    = nullptr;
        _other.m_byte_size_ = 0;
    }

    void cDynamic_Buffer::AlignAs( const size_t _align )
    {
        const bool is_safe_align = ( _align % Memory::kShaderAlign ) == 0;
        SK_WARN_IFN( sk::Severity::kConstGraphics | 10,
            is_safe_align, TEXT( "WARNING: The type isn't aligned by 16 bits. Doing this is considered undefined behaviour." ) )

        m_type_size_ = _align;
    }

    void cDynamic_Buffer::Resize( const size_t _size )
    {
        m_byte_size_ = m_type_size_ * _size;
        m_buffer_->Resize( m_byte_size_ );
    } // Resize
} // sk::Graphics
