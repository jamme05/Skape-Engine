/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Unsafe_Buffer.h"

#include <Debugging/Debugging.h>
#include <Memory/Tracker/Tracker.h>

#include <glbinding/gl/gl.h>

#include <print>

#include "Graphics/Renderer_Impl.h"

namespace sk::Graphics
{
    cUnsafe_Buffer::cUnsafe_Buffer() = default;

    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, const size_t _stride, Buffer::eType _type, const bool _is_normalized, const bool _is_static )
    : cUnsafe_Buffer( std::move( _name ), _byte_size, _stride, kTypeConverter[ static_cast< size_t >( _type ) ], _is_normalized, _is_static )
    {} // cUnsafe_Buffer

    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, const size_t _stride, gl::GLenum _type, const bool _is_normalized, const bool _is_static )
    : m_flags_( kInitialized | ( _is_normalized ? kNormalized : kNone ) | ( _is_static ? kStatic : kNone ) )
    , m_buffer_{ .type = _type, .size = _byte_size }
    , m_byte_size_( _byte_size )
    , m_stride_( _stride )
    , m_name_( std::move( _name ) )
    {
        create();
    } // cUnsafe_Buffer

    cUnsafe_Buffer::~cUnsafe_Buffer()
    {
        Destroy();
    } // ~cUnsafe_Buffer

    cUnsafe_Buffer& cUnsafe_Buffer::operator=( const cUnsafe_Buffer& _other )
    {
        if( this != &_other )
            return *this;

        Destroy();

        // If the other isn't initialized there's nothing to copy.
        if( !_other.IsInitialized() )
            return *this;

        m_flags_     = _other.m_flags_;
        m_byte_size_ = _other.m_byte_size_;
        m_name_      = _other.m_name_ + " Copy";

        create();

        if( m_byte_size_ > 0 )
            copy( _other );

        return *this;
    } // operator= ( Copy )

    cUnsafe_Buffer& cUnsafe_Buffer::operator=( cUnsafe_Buffer&& _other ) noexcept
    {
        Destroy();

        // If the other isn't initialized there's nothing to copy.
        if( !_other.IsInitialized() )
            return *this;

        // Should be safe?
        m_flags_     = _other.m_flags_;
        m_byte_size_ = _other.m_byte_size_;
        m_name_      = std::move( _other.m_name_ );
        m_buffer_    = _other.m_buffer_;
        m_data_      = _other.m_data_;
        _other.m_buffer_ = {};
        _other.m_data_ = nullptr;
        _other.m_flags_ &= ~kInitialized;

        return *this;
    } // operator=
    bool cUnsafe_Buffer::IsInitialized() const
    {
        return m_flags_ & kInitialized;
    }

    bool cUnsafe_Buffer::IsNormalized() const
    {
        return m_flags_ & kNormalized;
    }

    bool cUnsafe_Buffer::IsStatic() const
    {
        return m_flags_ & kStatic;
    }

    void cUnsafe_Buffer::create()
    {
        cGLRenderer::AddGLTask( [ & ]
        {
            gl::glGenBuffers( 1, &m_buffer_.buffer );
            gl::glBindBuffer( m_buffer_.type, m_buffer_.buffer );

            m_flags_ |= kInitialized;

            gl::glNamedBufferData( m_buffer_.buffer, static_cast< gl::GLsizeiptr >( m_byte_size_ ), nullptr,
                IsStatic() ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );

            if( m_byte_size_ > 0 )
                m_data_ = SK_ALLOC( m_byte_size_ );
        } );
    } // Create

    void cUnsafe_Buffer::copy( const cUnsafe_Buffer& _other )
    {
        // TODO: Warning message
        SK_WARN_IF( sk::Severity::kGraphics | 100,
            m_byte_size_ != _other.m_byte_size_, TEXT( "WARNING: " ) )

        m_is_updated_.store( true );
        memcpy( m_data_, _other.m_data_, std::min( m_byte_size_, _other.m_byte_size_ ) );

        // TODO: Make a copy function?
        gl::glBindBuffer( gl::GLenum::GL_COPY_READ_BUFFER, _other.m_buffer_.buffer );
        gl::glBindBuffer( gl::GLenum::GL_COPY_WRITE_BUFFER, m_buffer_.buffer );
        gl::glCopyBufferSubData( gl::GLenum::GL_COPY_READ_BUFFER, gl::GLenum::GL_COPY_WRITE_BUFFER,
            0, 0, static_cast< gl::GLsizeiptr >( m_byte_size_ ) );
    } // Copy

    void cUnsafe_Buffer::Destroy()
    {
        SK_FREE( m_data_ );
        m_data_ = nullptr;

        if( IsInitialized() )
            gl::glDeleteBuffers( 1, &m_buffer_.buffer );

        m_flags_ &= ~kInitialized;
    } // Free

    void cUnsafe_Buffer::Clear()
    {
        if( m_data_ != nullptr )
            std::memset( m_data_, 0, m_byte_size_ );
    } // Clear

    auto cUnsafe_Buffer::Data() -> void*
    {
        m_is_updated_.store( true );
        return m_data_;
    } // Data
    
    auto cUnsafe_Buffer::Data() const -> void*
    {
        return m_data_;
    } // Data

    cUnsafe_Buffer::cUnsafe_Buffer( const cUnsafe_Buffer& _other )
    : cUnsafe_Buffer( _other.m_name_ + " Copy", _other.m_byte_size_, _other.m_stride_, _other.m_buffer_.type, _other.IsNormalized(), _other.IsStatic() )
    {
        // Copy the buffer after creation.
        copy( _other );
    } // cUnsafe_Buffer ( Copy )

    // Should be safe?
    cUnsafe_Buffer::cUnsafe_Buffer( cUnsafe_Buffer&& _other ) noexcept
    : m_flags_( _other.m_flags_ )
    , m_buffer_( _other.m_buffer_ )
    , m_byte_size_( _other.m_byte_size_ )
    , m_data_( _other.m_data_ )
    , m_name_( std::move( _other.m_name_ ) )
    {
        m_is_updated_.store( true );
        _other.m_buffer_ = {};
        _other.m_data_    = nullptr;
        _other.m_flags_   = 0;
    } // cUnsafe_Buffer ( Move )

    void cUnsafe_Buffer::Read( void* _out, size_t _max_size ) const
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 10,
            m_data_ == nullptr, TEXT( "ERROR: Trying to read from empty buffer." ) )

        SK_BREAK_RET_IF( sk::Severity::kGraphics | 10,
            IsStatic(), TEXT( "ERROR: Trying to read from empty buffer." ) )

        if( _max_size == 0 )
            _max_size = m_byte_size_;
        else
            _max_size = std::min( _max_size, m_byte_size_ );

        memcpy( _out, m_data_, _max_size );
    } // Get

    void cUnsafe_Buffer::ReadRaw( void* _out, size_t _max_size ) const
    {
        if( _max_size == 0 )
            _max_size = m_buffer_.size;
        else
            _max_size = std::min( _max_size, m_buffer_.size );
        
        gl::glGetNamedBufferSubData( m_buffer_.buffer, 0, static_cast< gl::GLsizeiptr >(
            ( _max_size == 0 ? m_byte_size_ : Math::min( m_byte_size_, _max_size ) ) ), _out );
    } // ReadRaw

    void cUnsafe_Buffer::Update( const void* _data, const size_t _size )
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100,
            IsStatic() && m_byte_size_ > 0, TEXT( "ERROR: The buffer is static." ) )
        
        m_data_ = SK_REALLOC( m_data_, _size );
        m_byte_size_ = _size;

        m_is_updated_.store( true );
        memcpy( m_data_, _data, _size );
    } // Update

    void cUnsafe_Buffer::UpdateSeg( const void* _data, const size_t _size, const size_t _offset )
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 10,
            m_byte_size_ == 0, TEXT( "ERROR: Trying to set segment on empty Buffer." ) )

        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100,
            IsStatic(), TEXT( "ERROR: The buffer is static." ) )

        // These are separated to ease development.
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100, m_byte_size_ < _size,
            TEXT( "ERROR: The segment size is larger than the buffers size." ) )

        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100, m_byte_size_ < ( _size + _offset ),
            TEXT( "ERROR: The segment reaches outside of the buffer." ) )
        
        m_is_updated_.store( true );
        memcpy( static_cast< std::byte* >( m_data_ ) + _offset, _data, _size );
    } // UpdateSeg

    void cUnsafe_Buffer::Resize( const size_t _byte_size )
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100,
            IsStatic(), TEXT( "ERROR: The buffer is static." ) )

        if( m_byte_size_ == _byte_size )
            return;
        
        m_byte_size_ = _byte_size;
        
        m_is_updated_.store( true );
        m_data_ = SK_REALLOC( m_data_, _byte_size );
    } // Resize

    void cUnsafe_Buffer::SetStride( const size_t _new_stride )
    {
        // TODO: Ensure that the new stride is valid.
        m_stride_ = _new_stride;
    }

    void cUnsafe_Buffer::Upload( const bool _force )
    {
        SK_BREAK_RET_IF( sk::Severity::kConstGraphics | 10,
            m_byte_size_ == 0, TEXT( "ERROR: Trying to upload empty buffer." ) )

        if( _force || m_is_updated_ )
        {
            if( m_byte_size_ == m_buffer_.size )
                gl::glNamedBufferSubData( m_buffer_.buffer, 0,
                    static_cast< gl::GLsizeiptr >( m_byte_size_ ), m_data_ );
            else
            {
                glNamedBufferData( m_buffer_.buffer, static_cast< gl::GLsizeiptr >( m_byte_size_ ), m_data_,
                    IsStatic() ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );
                
                m_buffer_.size = m_byte_size_;
            }
            
            m_is_updated_.store( false );
        }
    } // Lock

    void cUnsafe_Buffer::Copy( const iUnsafe_Buffer& _other )
    {
        Resize( _other.GetSize() );

        // This is fine thanks to this being the only unsafe buffer class.
        copy( static_cast< const cUnsafe_Buffer& >( _other ) );
    } // Copy

    void cUnsafe_Buffer::Steal( iUnsafe_Buffer&& _other ) noexcept
    {
        // This is fine thanks to this being the only unsafe buffer class.
        *this = std::move( static_cast< cUnsafe_Buffer&& >( std::move( _other ) ) );
    }
} // sk::Graphics
