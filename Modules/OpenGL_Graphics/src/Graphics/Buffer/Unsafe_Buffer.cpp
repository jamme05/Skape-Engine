/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Unsafe_Buffer.h"

#include <Debugging/Debugging.h>
#include <Memory/Tracker/Tracker.h>

#include <print>

#include <stacktrace>

namespace sk::Graphics
{
    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, Buffer::eType _type, const bool _is_static )
    : cUnsafe_Buffer( std::move( _name ), _byte_size, kTypeConverter[ static_cast< size_t >( _type ) ], _is_static )
    {} // cUnsafe_Buffer

    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, gl::GLenum _type, const bool _is_static )
    : m_is_static_( _is_static )
    , m_type_( kTypeConverter[ static_cast< size_t >( _type ) ] )
    , m_size_( _byte_size )
    , m_name_( std::move( _name ) )
    {
        Create();
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

        m_is_static_ = _other.m_is_static_;
        m_type_      = _other.m_type_;
        m_size_      = _other.m_size_;
        m_name_      = _other.m_name_ + " Copy";

        Create();
        Copy( _other );

        return *this;
    } // operator= ( Copy )

    cUnsafe_Buffer& cUnsafe_Buffer::operator=( cUnsafe_Buffer&& _other ) noexcept
    {
        Destroy();

        // Should be safe?
        m_is_static_ = _other.m_is_static_;
        m_type_      = _other.m_type_;
        m_size_      = _other.m_size_;
        m_name_      = std::move( _other.m_name_ );
        m_buffer_    = _other.m_buffer_;

        return *this;
    } // operator=

    void cUnsafe_Buffer::Create()
    {
        gl::glGenBuffers( 1, &m_buffer_ );
        gl::glBindBuffer( m_type_, m_buffer_ );

        // TODO: Have the buffer always have a cpu copy IF it is dynamic. Otherwise it'll only have a gpu version.
        // This should in theory save performance and give static a better role.
        if( m_size_ > 0 )
            gl::glNamedBufferData( m_buffer_, static_cast< gl::GLsizeiptr >( m_size_ ), nullptr,
                m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );
    } // Create

    void cUnsafe_Buffer::Copy( const cUnsafe_Buffer& _other )
    {
        if( _other.m_is_locked_ )
            Update( _other.m_backup_data_, m_size_ );
        else
        {
            // Maybe make a copy function?
            gl::glBindBuffer( gl::GLenum::GL_COPY_READ_BUFFER, _other.m_buffer_ );
            gl::glBindBuffer( gl::GLenum::GL_COPY_WRITE_BUFFER, m_buffer_ );
            gl::glCopyBufferSubData( gl::GLenum::GL_COPY_READ_BUFFER, gl::GLenum::GL_COPY_WRITE_BUFFER,
                0, 0, static_cast< gl::GLsizeiptr >( m_size_ ) );
        }
    } // Copy

    void cUnsafe_Buffer::Destroy() const
    {
        if( m_has_backup_ )
            Memory::Tracker::free( m_backup_data_ );

        gl::glDeleteBuffers( 1, &m_buffer_ );
    } // Free

    cUnsafe_Buffer::cUnsafe_Buffer( const cUnsafe_Buffer& _other )
    : cUnsafe_Buffer( _other.m_name_ + " Copy", _other.m_size_, _other.m_type_, _other.m_is_static_ )
    {
        // Copy the buffer after creation.
        Copy( _other );
    } // cUnsafe_Buffer ( Copy )

    // Should be safe?
    cUnsafe_Buffer::cUnsafe_Buffer( cUnsafe_Buffer&& _other ) noexcept
    : m_is_static_( _other.m_is_static_ )
    , m_type_( _other.m_type_ )
    , m_buffer_( _other.m_buffer_ )
    , m_size_( _other.m_size_ )
    , m_name_( std::move( _other.m_name_ ) )
    {
    } // cUnsafe_Buffer ( Move )

    void cUnsafe_Buffer::Read( void* _out, const size_t _max_size ) const
    {
        // Backup doesn't care about access.
        if( m_has_backup_ && m_is_locked_ )
            memcpy( _out, m_backup_data_, ( _max_size == 0 ? m_backup_size_ : Math::min( m_backup_size_, _max_size ) ) );
        else
            ReadRaw( _out, _max_size );
    } // Get

    void cUnsafe_Buffer::ReadRaw( void* _out, const size_t _max_size ) const
    {
        gl::glGetNamedBufferSubData( m_buffer_, 0, static_cast< gl::GLsizeiptr >(
            ( _max_size == 0 ? m_size_ : Math::min( m_size_, _max_size ) ) ), _out );
    } // ReadRaw

    void cUnsafe_Buffer::Update( const void* _data, const size_t _size )
    {
        SK_BREAK_IF_RET( sk::Severity::kGraphics | 100,
            m_is_static_, TEXT( "WARNING: The buffer is static." ) )

        std::lock_guard lock( m_write_mtx_ );
        if( m_is_locked_ )
        {
            if( m_has_backup_ )
                create_backup( _size );
            else
            {
                m_backup_data_ = Memory::Tracker::realloc( m_backup_data_, _size );
                m_backup_size_ = _size;
            }

            memcpy( m_backup_data_, _data, _size );
        }
        else
        {
            // As per https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferSubData.xhtml
            // Update entire buffer as sub data to skip reallocation if no resize has happened.
            if( m_size_ == _size )
                gl::glNamedBufferSubData( m_buffer_, 0,
                static_cast< gl::GLsizeiptr >( m_size_ ), _data );
            else
                glNamedBufferData( m_buffer_, static_cast< gl::GLsizeiptr >( m_size_ ), _data,
                    m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );
        }
    } // Update

    void cUnsafe_Buffer::UpdateSeg( const void* _data, const size_t _size, const size_t _offset )
    {
        SK_BREAK_IF_RET( sk::Severity::kGraphics | 100,
            m_is_static_, TEXT( "WARNING: The buffer is static." ) )

        // Copy value to not require store.
        std::lock_guard lock( m_write_mtx_ );
        const bool        is_locked = m_is_locked_;
        const auto size = is_locked ? m_backup_size_ : m_size_;

        // These are separated to ease development.
        SK_BREAK_IF_RET( sk::Severity::kGraphics | 10000, size < _size,
            TEXT( "WARNING: The segment size is larger than the buffers size." ) )

        SK_BREAK_IF_RET( sk::Severity::kGraphics | 500, size < ( _size + _offset ),
            TEXT( "WARNING: The segment reaches outside of the buffer." ) )

        if( is_locked )
        {
            // Only create backup if it doesn't exist.
            if( !m_has_backup_ )
                create_backup();
            else
            {
                m_backup_data_ = Memory::Tracker::realloc( m_backup_data_, _size );
                m_backup_size_ = _size;
            }

            // TODO: Fix manual variant of glNamedBufferSubData
            memcpy( m_backup_data_ + _offset, _data, _size );
        }
        else
        {
            gl::glNamedBufferSubData( m_buffer_, static_cast< gl::GLintptr >( _offset ),
                static_cast< gl::GLsizeiptr >( m_size_ ), _data );
        }
    } // UpdateSeg

    size_t cUnsafe_Buffer::GetSafeSize() const
    {
        return ( m_is_locked_.load() ? m_backup_size_ : m_size_ );
    } // GetSafeSize

    void cUnsafe_Buffer::Resize( const size_t _byte_size )
    {
        SK_WARN_IF( sk::Severity::kGraphics | 100,
            m_is_static_, TEXT( "WARNING: The buffer is static." ) )

        std::lock_guard lock( m_write_mtx_ );
        if( m_is_locked_ )
        {
            if( m_backup_size_ == _byte_size )
                return;

            if( m_has_backup_ )
                m_backup_data_ = Memory::Tracker::realloc( m_backup_data_, _byte_size );
            else
                create_backup( _byte_size, true );
        }
        else if( m_size_ != _byte_size )
        {
            // Copy the old buffer.
            const auto tmp = Memory::Tracker::alloc( m_size_ );
            gl::glGetNamedBufferSubData( m_buffer_, 0, static_cast< gl::GLsizeiptr >( m_size_ ), tmp );

            gl::glNamedBufferData( m_buffer_, static_cast< gl::GLsizeiptr >( m_size_ ),
                tmp, m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );
        }
    } // Resize

    void cUnsafe_Buffer::Lock()
    {
        if( m_is_locked_ )
        {
            [[ unlikely ]]
            throw std::runtime_error( "The buffer is already locked." );
        }

        m_is_locked_.store( true );
    } // Lock

    void cUnsafe_Buffer::Unlock()
    {
        if( m_is_locked_ )
        {
            std::lock_guard lock( m_write_mtx_ );
            m_has_backup_ = false;
            m_is_locked_.store( false );

            // Only upload backup if it exists.
            if( !m_has_backup_ )
                return;

            // Copied from update.
            if( m_size_ == m_backup_size_ )
                gl::glNamedBufferSubData( m_buffer_, 0,
                static_cast< gl::GLsizeiptr >( m_size_ ), m_backup_data_ );
            else
                glNamedBufferData( m_buffer_, static_cast< gl::GLsizeiptr >( m_backup_size_ ), m_backup_data_,
                    m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );

            Memory::Tracker::free( m_backup_data_ );
            m_backup_data_ = nullptr;
        }
        else
            SK_WARNING( sk::Severity::kGraphics,
                "WARNING: The buffer was already unlocked." )
    } // Unlock

    void cUnsafe_Buffer::create_backup( size_t _size, const bool _force_copy )
    {
        // If a size is provided, we expect the user to fill the data themselves.
        bool copy_buffer = _force_copy;
        if( _size == 0 )
        {
            _size       = m_size_;
            copy_buffer = true;
        }

        m_backup_data_ = Memory::Tracker::alloc( _size );
        m_has_backup_  = true;

        if( copy_buffer )
            gl::glGetNamedBufferSubData( m_buffer_, 0,
                static_cast< gl::GLsizeiptr >( Math::min( m_size_, _size ) ),
                m_backup_data_ );

        // Update size last to allow for a safe forced copy.
        m_backup_size_ = _size;
    } // alloc_backup
} // sk::Graphics
