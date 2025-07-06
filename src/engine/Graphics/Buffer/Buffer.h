/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer.h>

// TYPE SAFE BUFFER

namespace sk::Graphics
{
    template< class Ty, Buffer::eType Type >
    class cBuffer
    {
    public:
        using value_type = Ty;

        cBuffer();
        cBuffer( const cBuffer& ) = default;
        cBuffer( cBuffer&& ) noexcept = default;
        ~cBuffer();
    private:
        cUnsafe_Buffer m_buffer_;
    };

    template< class Ty >
    using cConstantBuffer = cBuffer< Ty, Buffer::eType::kConstant >;
    template< class Ty >
    using cStructedBuffer = cBuffer< Ty, Buffer::eType::kStructed >;
    template< class Ty >
    using cVertexBuffer = cBuffer< Ty, Buffer::eType::kVertex >;
} // sk::Graphics::
