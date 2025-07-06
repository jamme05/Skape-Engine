/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer.h>

#include <Reflection/Types.h>
#include <Math/Types.h>

// TYPE UNSAFE BUFFER

namespace sk::Graphics
{
    class cDynamic_Buffer
    {
    public:
        cDynamic_Buffer( Buffer::eType _type, const std::string& _name );

        cDynamic_Buffer( const cDynamic_Buffer& _other );
        cDynamic_Buffer( cDynamic_Buffer&& _other ) noexcept;

        ~cDynamic_Buffer();
    private:
        // The outwards size of this buffer. Get the size from the unsafe buffer for the capacity.
        size_t         m_size_;
        // Uses both.
        size_t         m_type_size_;
        type_info_t    m_type_;

        cUnsafe_Buffer m_buffer_;
    };
} // sk::Graphics

void t()
{
    sk::Graphics::cDynamic_Buffer buffer = sk::Graphics::cDynamic_Buffer{ sk::Graphics::Buffer::eType::kConstant, "Test" };
}