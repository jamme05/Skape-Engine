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

namespace sk::Graphics
{
    cUnsafe_Buffer::cUnsafe_Buffer()
    : m_is_static_( false )
    {} // cUnsafe_Buffer

    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, Buffer::eType _type, const bool _is_static )
    : cUnsafe_Buffer( std::move( _name ), _byte_size, kTypeConverter[ static_cast< size_t >( _type ) ], _is_static )
    {} // cUnsafe_Buffer

    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, gl::GLenum _type, const bool _is_static )
    : m_is_static_( _is_static )
    , m_buffer_{ .type = kTypeConverter[ static_cast< size_t >( _type ) ], .size = _byte_size }
    , m_byte_size_( _byte_size )
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
        if( !_other.m_is_initialized_ )
            return *this;

        m_is_static_ = _other.m_is_static_;
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
        if( !_other.m_is_initialized_ )
            return *this;

        // Should be safe?
        m_is_static_ = _other.m_is_static_;
        m_byte_size_ = _other.m_byte_size_;
        m_name_      = std::move( _other.m_name_ );
        m_buffer_    = _other.m_buffer_;
        m_data_      = _other.m_data_;

        return *this;
    } // operator=

    void cUnsafe_Buffer::create()
    {
        gl::glGenBuffers( 1, &m_buffer_.buffer );
        gl::glBindBuffer( m_buffer_.type, m_buffer_.buffer );

        m_is_initialized_ = true;

        if( m_byte_size_ > 0 )
        {
            gl::glNamedBufferData( m_buffer_.buffer, static_cast< gl::GLsizeiptr >( m_byte_size_ ), nullptr,
                m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );

            m_data_ = SK_ALLOC( m_byte_size_ );
        }
    } // Create

    void cUnsafe_Buffer::copy( const cUnsafe_Buffer& _other )
    {
        std::lock_guard lock( m_write_mtx_ );

        // TODO: Warning message
        SK_WARN_IF( sk::Severity::kGraphics | 100,
            m_byte_size_ != _other.m_byte_size_, TEXT( "WARNING: " ) )

        m_is_updated_.store( true );
        memcpy( m_data_, _other.m_data_, std::min( m_byte_size_, _other.m_byte_size_ ) );

        if( !m_is_locked_ )
        {
            // Maybe make a copy function?
            gl::glBindBuffer( gl::GLenum::GL_COPY_READ_BUFFER, _other.m_buffer_.buffer );
            gl::glBindBuffer( gl::GLenum::GL_COPY_WRITE_BUFFER, m_buffer_.buffer );
            gl::glCopyBufferSubData( gl::GLenum::GL_COPY_READ_BUFFER, gl::GLenum::GL_COPY_WRITE_BUFFER,
                0, 0, static_cast< gl::GLsizeiptr >( m_byte_size_ ) );
        }
    } // Copy

    void cUnsafe_Buffer::Destroy()
    {
        SK_FREE( m_data_ );

        if( m_is_initialized_ )
            gl::glDeleteBuffers( 1, &m_buffer_.buffer );

        m_is_initialized_ = false;
    } // Free

    void cUnsafe_Buffer::Clear()
    {
        std::memset( m_data_, 0, m_byte_size_ );
    } // Clear

    auto cUnsafe_Buffer::Get() -> void*
    {
        return m_data_;
    } // Get

    auto cUnsafe_Buffer::Get() const -> const void*
    {
        return m_data_;
    } // Get

    cUnsafe_Buffer::cUnsafe_Buffer( const cUnsafe_Buffer& _other )
    : cUnsafe_Buffer( _other.m_name_ + " Copy", _other.m_byte_size_, _other.m_buffer_.type, _other.m_is_static_ )
    {
        // Copy the buffer after creation.
        copy( _other );
    } // cUnsafe_Buffer ( Copy )

    // Should be safe?
    cUnsafe_Buffer::cUnsafe_Buffer( cUnsafe_Buffer&& _other ) noexcept
    : m_is_static_( _other.m_is_static_ )
    , m_buffer_( _other.m_buffer_ )
    , m_byte_size_( _other.m_byte_size_ )
    , m_data_( _other.m_data_ )
    , m_name_( std::move( _other.m_name_ ) )
    {
        m_is_updated_.store( true );
    } // cUnsafe_Buffer ( Move )

    void cUnsafe_Buffer::Read( void* _out, size_t _max_size ) const
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 10,
            m_data_ == nullptr, TEXT( "ERROR: Trying to read from empty buffer." ) )

        SK_BREAK_RET_IF( sk::Severity::kGraphics | 10,
            m_is_static_, TEXT( "ERROR: Trying to read from empty buffer." ) )

        if( _max_size == 0 )
            _max_size = m_byte_size_;
        else
            _max_size = std::min( _max_size, m_byte_size_ );

        if( m_is_locked_ )
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
            m_is_static_ && m_byte_size_ > 0, TEXT( "ERROR: The buffer is static." ) )

        std::scoped_lock lock( m_write_mtx_ );

        m_data_ = Memory::Tracker::realloc( m_data_, _size );
        m_byte_size_ = _size;

        m_is_updated_.store( true );
        memcpy( m_data_, _data, _size );
    } // Update

    void cUnsafe_Buffer::UpdateSeg( const void* _data, const size_t _size, const size_t _offset )
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 10,
            m_byte_size_ == 0, TEXT( "ERROR: Trying to set segment on empty Buffer." ) )

        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100,
            m_is_static_, TEXT( "ERROR: The buffer is static." ) )

        // These are separated to ease development.
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100, m_byte_size_ < _size,
            TEXT( "ERROR: The segment size is larger than the buffers size." ) )

        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100, m_byte_size_ < ( _size + _offset ),
            TEXT( "ERROR: The segment reaches outside of the buffer." ) )

        std::scoped_lock lock( m_write_mtx_ );
        m_is_updated_.store( true );
        memcpy( static_cast< std::byte* >( m_data_ ) + _offset, _data, _size );
    } // UpdateSeg

    void cUnsafe_Buffer::Resize( const size_t _byte_size )
    {
        SK_BREAK_RET_IF( sk::Severity::kGraphics | 100,
            m_is_static_, TEXT( "ERROR: The buffer is static." ) )

        std::scoped_lock lock( m_write_mtx_ );
        if( m_byte_size_ == _byte_size )
            return;

        m_is_updated_.store( true );
        m_data_ = SK_REALLOC( m_data_, _byte_size );
    } // Resize

    void cUnsafe_Buffer::Lock()
    {
        if( m_is_locked_ )
        {
            [[ unlikely ]]
            throw std::runtime_error( "The buffer is already locked." );
        }

        SK_BREAK_RET_IF( sk::Severity::kConstGraphics | 10,
            m_byte_size_ == 0, TEXT( "ERROR: Trying to lock empty buffer." ) )

        if( m_is_locked_ )
        {
            SK_BREAK_IF( sk::Severity::kConstGraphics,
               true, TEXT( "ERROR: The buffer is already locked." ) )
        }
        else if( m_is_updated_ )
        {
            std::scoped_lock lock( m_write_mtx_ );
            if( m_byte_size_ == m_buffer_.size )
                gl::glNamedBufferSubData( m_buffer_.buffer, 0,
                static_cast< gl::GLsizeiptr >( m_byte_size_ ), m_data_ );
            else
                glNamedBufferData( m_buffer_.buffer, static_cast< gl::GLsizeiptr >( m_byte_size_ ), m_data_,
                    m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );

            m_is_locked_.store( true );
        }
    } // Lock

    void cUnsafe_Buffer::Unlock()
    {
        if( m_is_locked_ )
            m_is_locked_.store( false );
        else
            SK_WARNING( sk::Severity::kGraphics | 100,
                "WARNING: The buffer was already unlocked." )
    } // Unlock

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
