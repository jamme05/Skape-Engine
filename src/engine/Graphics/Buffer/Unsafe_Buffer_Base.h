/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once
#include <cstdint>

// Will be overriden in Module
// The override SHOULD be at sk::Graphics::cUnsafe_Buffer
// The unsafe buffer will be acting as the allocator for buffers.

namespace sk::Graphics
{
    class iUnsafe_Buffer
    {
    public:
        enum class eType : uint8_t
        {
            kConstant,
            kStructed,
            kVertex
        };

        enum eAccess : uint8_t
        {
            kNone      = 0x00,
            kRead      = 0x01,
            kWrite     = 0x02,
            kReadWrite = kRead | kWrite
        };
        virtual ~iUnsafe_Buffer( void ) = 0;

        virtual void   Read   ( void* _out,  size_t _map_size ) = 0;
        virtual void   Update ( void* _data, size_t _size, size_t _offset = 0 ) = 0;
        [[ nodiscard ]]
        virtual size_t GetSize( void ) const = 0;
        virtual void   Resize ( size_t _byte_size ) = 0;

        virtual void  Lock    ( void ) = 0;
        virtual void  Unlock  ( void ) = 0;
        [[ nodiscard ]]
        virtual bool  IsLocked( void ) const = 0;
    };
} // sk::Graphics::
