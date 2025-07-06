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
        static constexpr auto kType     = Type;
        static constexpr auto kTypeSize = sizeof( Ty );
        using value_type = Ty;

        explicit cBuffer( std::string _name, const bool _is_static = false )
        : m_buffer_( std::move( _name ), 0, kType, _is_static )
        {} // cBuffer

        cBuffer( const cBuffer& _other );
        cBuffer( cBuffer&& _other ) noexcept;
        ~cBuffer();
    private:
        cUnsafe_Buffer m_buffer_;
    };

    template< class Ty >
    using cConstant_Buffer = cBuffer< Ty, Buffer::eType::kConstant >;
    template< class Ty >
    using cStructed_Buffer = cBuffer< Ty, Buffer::eType::kStructed >;
    template< class Ty >
    using cVertex_Buffer = cBuffer< Ty, Buffer::eType::kVertex >;

    
} // sk::Graphics::
