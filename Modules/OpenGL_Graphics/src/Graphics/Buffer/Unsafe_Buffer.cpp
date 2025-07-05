/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Unsafe_Buffer.h"

#include <iostream>
#include <print>
#include <Memory/Tracker/Tracker.h>

namespace sk::Graphics
{
    cUnsafe_Buffer::cUnsafe_Buffer( std::string _name, const size_t _byte_size, eType _type, const bool _is_static )
    : m_is_static_( _is_static )
    , m_type_( kTypeConverter[ static_cast< size_t >( _type ) ] )
    , m_size_( _byte_size )
    , m_name_( std::move( _name ) )
    {
        gl::glCreateBuffers( 1, &m_buffer_ );

        if( m_size_ > 0 )
            gl::glNamedBufferData( m_buffer_, static_cast< gl::GLsizeiptr >( m_size_ ), nullptr,
                m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );
    } // cUnsafe_Buffer

    void cUnsafe_Buffer::Read( void* _out, const size_t _max_size )
    {
        // Backup doesn't care about access.
        if( m_is_locked_ )
            memcpy( _out, m_backup_data_, ( _max_size == 0 ? m_backup_size_ : Math::min( m_backup_size_, _max_size ) ) );
        else
            ReadRaw( _out, _max_size );
    } // Get

    void cUnsafe_Buffer::ReadRaw( void* _out, const size_t _max_size )
    {
        // TODO: Look over this formatting.
        gl::glGetNamedBufferSubData( m_buffer_, 0, static_cast< gl::GLsizeiptr >(
            ( _max_size == 0 ? m_size_ : Math::min( m_size_, _max_size ) ) )
            , _out );
    } // ReadRaw

    void cUnsafe_Buffer::Update( void* _data, const size_t _size )
    {
        if( m_is_static_ )
            std::print( "WARNING: The buffer is locked." );

        if( m_is_locked_ )
        {
            if( m_is_changed_ )
                create_backup( _size );
            else
            {
                m_backup_data_ = Memory::Tracker::realloc( m_backup_data_, _size );
                m_backup_size_ = _size;
            }

            memcpy( _data, m_backup_data_, _size );
        }
        else
        {
            glNamedBufferData( m_buffer_, static_cast< gl::GLsizeiptr >( m_size_ ), _data,
                m_is_static_ ? gl::GLenum::GL_STATIC_DRAW : gl::GLenum::GL_DYNAMIC_DRAW );
        }
    } // Update

    void cUnsafe_Buffer::UpdateSeg( void* _data, size_t _size, size_t _offset )
    {
        if( m_is_static_ )
            std::print( "WARNING: The buffer is locked." );

        if( m_is_locked_ )
        {
            if( m_is_changed_ )
                create_backup( _size );
            else
            {
                m_backup_data_ = Memory::Tracker::realloc( m_backup_data_, _size );
                m_backup_size_ = _size;
            }

            // TODO: Fix manual variant of glNamedBufferSubData
            memcpy( _data, m_backup_data_, _size );
        }
        else
        {
            gl::glNamedBufferSubData( m_buffer_, static_cast< gl::GLintptr >( _offset ),
                static_cast< gl::GLsizeiptr >( m_size_ ), _data );
        }

    } // UpdateSeg

    void cUnsafe_Buffer::Resize( size_t _byte_size )
    {
        if( m_is_locked_ )
        {
            // TODO: Fix resizing. Should resizing be a thing as well?
        }
    } // Resize

    void cUnsafe_Buffer::Lock()
    {
        if( m_is_locked_ )
            throw std::runtime_error( "The buffer is  already locked." );

        m_is_locked_.store( true );
    } // Lock

    void cUnsafe_Buffer::Unlock()
    {
        if( m_is_locked_ )
        {
            // TODO: Copy data from backup into main buffer.
        }
        else
            std::println( stderr, "WARNING: The buffer was already unlocked." );

        m_is_locked_.store( false );
    } // Unlock

    void cUnsafe_Buffer::create_backup( size_t _size )
    {
        // If a size is provided, we expect the user to fill the data themselves.
        bool copy_buffer = false;
        if( _size == 0 )
        {
            _size = m_size_;
            copy_buffer = true;
        }

        m_backup_data_ = Memory::Tracker::alloc( _size );
        m_backup_size_ = _size;

        if( copy_buffer )
            gl::glGetNamedBufferSubData( m_buffer_, 0, static_cast< gl::GLsizeiptr >( m_backup_size_ ), m_backup_data_ );
    } // alloc_backup
} // sk::Graphics
