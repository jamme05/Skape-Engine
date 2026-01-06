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
    : m_raw_size_( 0 )
    , m_buffer_type_( Buffer::eType::kInvalid )
    , m_type_size_( 0 )
    , m_item_type_( nullptr )
    , m_buffer_( nullptr )
    {} // cDynamic_Buffer

    cDynamic_Buffer::cDynamic_Buffer( const std::string& _name, const Buffer::eType _type, const bool _is_normalized )
    : cDynamic_Buffer( _name, _type, _is_normalized, static_cast< size_t >( 0 ), static_cast< size_t >( 0 ) )
    {} // cDynamic_Buffer
    
    cDynamic_Buffer::cDynamic_Buffer( const std::string& _name, Buffer::eType _type, bool _is_normalized, size_t _raw_size, size_t _raw_stride )
    : m_raw_size_( 0 )
    , m_buffer_type_( _type )
    , m_type_size_( 0 )
    , m_item_type_( nullptr )
    , m_buffer_( std::make_unique< cUnsafe_Buffer >( _name, _raw_size, _raw_stride, _type, _is_normalized, false ) )
    {
        m_buffer_->Clear();
    }

    cDynamic_Buffer::cDynamic_Buffer( const cDynamic_Buffer& _other )
    : m_raw_size_( _other.m_raw_size_ )
    , m_buffer_type_( _other.m_buffer_type_ )
    , m_type_size_( _other.m_type_size_ )
    , m_item_type_( _other.m_item_type_ )
    , m_buffer_{ std::make_unique< cUnsafe_Buffer >(
        _other.m_buffer_->GetName(), m_raw_size_, m_type_size_, m_buffer_type_, _other.GetBuffer().IsNormalized(), false ) }
    {
        m_buffer_->Copy( *_other.m_buffer_ );
    }

    cDynamic_Buffer::cDynamic_Buffer( cDynamic_Buffer&& _other ) noexcept
    : m_raw_size_( _other.m_raw_size_ )
    , m_buffer_type_( _other.m_buffer_type_ )
    , m_type_size_( _other.m_type_size_ )
    , m_item_type_( _other.m_item_type_ )
    , m_buffer_{ std::move( _other.m_buffer_ ) }
    {
        _other.m_buffer_    = nullptr;
        _other.m_raw_size_ = 0;
    }

    cDynamic_Buffer::~cDynamic_Buffer()
    {
        m_buffer_ = nullptr;
    }

    cDynamic_Buffer& cDynamic_Buffer::operator=( const cDynamic_Buffer& _other )
    {
        if( this == &_other )
            return *this;
        
        m_raw_size_    = _other.m_raw_size_;
        m_buffer_type_ = _other.m_buffer_type_;
        m_type_size_   = _other.m_type_size_;
        m_item_type_   = _other.m_item_type_;
        
        m_buffer_ = std::make_unique< cUnsafe_Buffer >( _other.m_buffer_->GetName(), m_raw_size_, m_type_size_, m_buffer_type_, _other.GetBuffer().IsNormalized(), false );
        m_buffer_->Copy( *_other.m_buffer_ );
        
        return *this;
    }

    cDynamic_Buffer& cDynamic_Buffer::operator=( cDynamic_Buffer&& _other ) noexcept
    {
        m_raw_size_    = _other.m_raw_size_;
        m_buffer_type_ = _other.m_buffer_type_;
        m_type_size_   = _other.m_type_size_;
        m_item_type_   = _other.m_item_type_;
        
        m_buffer_ = std::move( _other.m_buffer_ );
        
        return *this;
    }

    void cDynamic_Buffer::UnsafeAlignAs( const size_t _align, bool _clear )
    {
        const bool is_safe_align = ( ( _align * 8 ) % Memory::kShaderAlign ) == 0;
        SK_WARN_IFN( sk::Severity::kConstGraphics | 10,
            is_safe_align, TEXT( "WARNING: The type isn't aligned by 16 bits. Doing this is considered undefined behaviour." ) )

        if( _clear )
            m_buffer_->Clear();
        
        m_type_size_ = _align;
        m_buffer_->SetStride( m_type_size_ );
    }

    void cDynamic_Buffer::AlignAs( const type_info_t _type, const bool _clear )
    {
        SK_ERR_IF( _type == nullptr,
            "Error: No type provided." )

        m_item_type_ = _type;

        UnsafeAlignAs( _type->size, _clear );
    }

    void* cDynamic_Buffer::RawData()
    {
        return m_buffer_->Data();
    }

    void* cDynamic_Buffer::RawData() const
    {
        return m_buffer_->Data();
    }

    void cDynamic_Buffer::Resize( const size_t _size )
    {
        m_raw_size_ = m_type_size_ * _size;
        m_buffer_->Resize( m_raw_size_ );
    } // Resize
    
    auto cDynamic_Buffer::GetSize() const -> size_t
    {
        return m_raw_size_ / m_type_size_;
    }

    auto cDynamic_Buffer::GetType() const -> Buffer::eType
    {
        return m_buffer_type_;
    }

    auto cDynamic_Buffer::GetItemSize() const -> size_t
    {
        return m_type_size_;
    }

    auto cDynamic_Buffer::GetItemType() const -> type_info_t
    {
        return m_item_type_;
    }

    auto cDynamic_Buffer::GetBuffer() const -> iUnsafe_Buffer&
    {
        return *m_buffer_;
    }

    bool cDynamic_Buffer::IsValid() const
    {
        if( m_buffer_ == nullptr )
            return false;

        return m_buffer_->IsInitialized();
    }
} // sk::Graphics
