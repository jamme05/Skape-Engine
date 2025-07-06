/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

// TYPE SAFE BUFFER

namespace sk::Graphics
{
    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer()
    : m_buffer(  )
    {
    }
} // sk::Graphics::
