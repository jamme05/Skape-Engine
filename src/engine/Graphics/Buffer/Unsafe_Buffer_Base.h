/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>
#include <string>

// Will be overriden in Module
// The override SHOULD be at sk::Graphics::cUnsafe_Buffer
// The header HAS to be in src/Graphics/Buffer/Unsafe_Buffer.h
// The unsafe buffer will be acting as the allocator for buffers.
// It is NOT recommended to use the unsafe buffer directly unless you know what you're doing.

namespace sk::Graphics
{
    namespace Buffer
    {
        enum class eType : uint8_t
        {
            kInvalid,
            kConstant,
            kIndex,
            kVertex,
            kStructed,
        };

        enum class eAccess : uint8_t
        {
            kRead,
            kWrite,
            kReadWrite,
        };
    } // Buffer::

    class iUnsafe_Buffer
    {
    public:
        virtual ~iUnsafe_Buffer( void ) = 0;

        virtual void Destroy() = 0;
        virtual void Clear() = 0;
        virtual auto Get()       -> void* = 0;
        virtual auto Get() const -> const void* = 0;
        /**
         * Will copy the active version of data from the Buffer to specified pointer.
         * @param _out Pointer to copy the data to.
         * @param _max_size Maximum allowed size. If 0 it'll use the buffers size. Default: 0
         */
        virtual void   Read   ( void* _out,  size_t _max_size = 0 ) const = 0;
        /**
         * Will copy the data from the gpu to specified pointer.
         * @param _out Pointer to copy the data to.
         * @param _max_size Maximum allowed size. If 0 it'll use the buffers size. Default: 0
         */
        virtual void   ReadRaw( void* _out,  size_t _max_size = 0 ) const = 0;
        /**
         * Update the data on the buffer.
         * @param _data Pointer to the data that is going to be copied to the buffer.
         * @param _size The new size of the buffer. NOTE: If the purpose is resizing. Use Resize instead.
         */
        virtual void   Update ( const void* _data, size_t _size ) = 0;
        /**
         * Update data within the buffer.
         * @param _data Pointer to the data that is going to be copied to the segment.
         * @param _size 
         * @param _offset 
         */
        virtual void   UpdateSeg( const void* _data, size_t _size, size_t _offset = 0 ) = 0;
        [[ nodiscard ]]
        virtual size_t GetSize( void ) const = 0;
        virtual void   Resize ( size_t _byte_size ) = 0;

        virtual void  Copy ( const iUnsafe_Buffer& _other ) = 0;
        virtual void  Steal( iUnsafe_Buffer&& _other ) noexcept = 0;

        virtual void  Lock    ( void ) = 0;
        virtual void  Unlock  ( void ) = 0;
        [[ nodiscard ]]
        virtual bool  IsLocked( void ) const = 0;

        virtual std::string GetName() const = 0;
    };
} // sk::Graphics::
