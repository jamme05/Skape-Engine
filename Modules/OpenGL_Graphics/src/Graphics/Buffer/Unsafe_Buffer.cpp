/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Unsafe_Buffer.h"

#include <print>
#include <Memory/Tracker/Tracker.h>

namespace sk::Graphics
{
    cUnsafe_Buffer::cUnsafe_Buffer( const std::string& _name, const size_t _byte_size, eType _type, const bool _is_static )
    : m_is_static_( _is_static )
    , m_type_( kTypeConverter[ static_cast< size_t >( _type ) ] )
    , m_size_( _byte_size )
    {
        glGenBuffers( 1, &m_buffer_ );
        glBindBuffer( m_type_, m_buffer_ );

        // Bind if size > 0
        glBufferData( m_type_, static_cast< GLsizeiptr >( m_size_ ), nullptr, m_is_static_ ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );
    } // cUnsafe_Buffer

    void cUnsafe_Buffer::Read( void* _out, const size_t _max_size )
    {
        // Backup doesn't care about access.
        if( m_is_locked_ )
            memcpy( _out, m_backup_data_, Math::min( m_size_, _max_size ) );
        else
        {
            glBindBuffer( m_type_, m_buffer_ );
            glGetBufferSubData( m_type_, 0, static_cast< GLsizeiptr >( Math::min( m_size_, _max_size ) ), _out );
        }
    } // Get

    void cUnsafe_Buffer::Update( void* _data, size_t _size, size_t _offset )
    {
        if( m_is_static_ )
            std::print( "WARNING: The buffer is locked." );
    } // Update

    void cUnsafe_Buffer::Resize( size_t _byte_size )
    {
    } // Resize
} // sk::Graphics
