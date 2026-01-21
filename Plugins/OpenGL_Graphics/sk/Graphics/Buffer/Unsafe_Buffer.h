/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <atomic>
#include <mutex>
#include <string>

#include <glbinding/gl/gl.h>

#include <Graphics/Buffer/Unsafe_Buffer_Base.h>

namespace sk::Graphics
{
    namespace OpenGL
    {
        // This is designed for being accessed by a rendering thread ( which locks/unlocks )
        // and a secondary thread which writes.
        class cUnsafe_Buffer final : public iUnsafe_Buffer
        {
            // REQUIRED.
            struct sRaw_Buffer
            {
                gl::GLenum type   = gl::GLenum::GL_INVALID_VALUE;
                gl::GLuint buffer = 0;
                size_t     size   = 0;
            };

            static constexpr gl::GLenum kTypeConverter[]
            {
                gl::GLenum::GL_INVALID_ENUM,
                // kConstant
                gl::GLenum::GL_UNIFORM_BUFFER, // https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
                // kUniform
                gl::GLenum::GL_ELEMENT_ARRAY_BUFFER,
                // kVertex
                gl::GLenum::GL_ARRAY_BUFFER, // https://learnopengl.com/Getting-started/Hello-Triangle#12
                // kStructed
                gl::GLenum::GL_SHADER_STORAGE_BUFFER, // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
            };
            
            static constexpr gl::GLenum kAccessConverter[]
            {
                gl::GLenum::GL_READ_ONLY,
                gl::GLenum::GL_WRITE_ONLY,
                gl::GLenum::GL_READ_WRITE
            };
        public:
            cUnsafe_Buffer();
            cUnsafe_Buffer( std::string _name, size_t _byte_size, size_t _stride, Buffer::eType _type, bool _is_normalized, bool _is_static );
            cUnsafe_Buffer( std::string _name, size_t _byte_size, size_t _stride, gl::GLenum _type, bool _is_normalized, bool _is_static );
            cUnsafe_Buffer( const cUnsafe_Buffer& _other );
            cUnsafe_Buffer( cUnsafe_Buffer&& _other ) noexcept;

            ~cUnsafe_Buffer() override;

            cUnsafe_Buffer& operator=( const cUnsafe_Buffer& _other );
            cUnsafe_Buffer& operator=( cUnsafe_Buffer&& _other ) noexcept;

            bool IsInitialized() const override;
            bool IsNormalized () const override;
            bool IsStatic     () const override;

            void Destroy() override;
            void Clear  () override;

            // Remove the raw get due to it avoiding all safety?
            auto Data() -> void* override;
            auto Data() const -> void* override;
            
            void Read   ( void* _out, size_t _max_size = 0 ) const override;
            void ReadRaw( void* _out, size_t _max_size = 0 ) const override;
            void Update ( const void* _data, size_t _size ) override;
            void UpdateSegment( const void* _data, size_t _size, size_t _offset ) override;
            [[ nodiscard ]]
            auto GetSize() const -> size_t override { return m_byte_size_; }
            void Resize ( size_t _byte_size ) override;
            
            auto GetStride() const -> size_t override { return m_stride_; }
            void SetStride( size_t _new_stride ) override;
            
            void SetChanged() override;
            void Upload( bool _force ) override;

            void Copy ( const iUnsafe_Buffer& _other ) override;
            void Steal( iUnsafe_Buffer&& _other ) noexcept override;

            std::string GetName() const override { return m_name_; }

            // DO NOT USE ANYWHERE EXCEPT FOR INTERNAL USAGE
            [[ nodiscard ]]
            auto get_buffer() const { return m_buffer_; }
        private:
            void create();
            void copy( const cUnsafe_Buffer& _other );

            // If the backup is in use.
            uint16_t         m_flags_      = 0;
            std::atomic_bool m_is_updated_ = true;

            sRaw_Buffer m_buffer_;

            // TODO: Allow the user to control the capacity more.
            size_t m_byte_size_ = 0;
            size_t m_capacity_  = 0;
            void*  m_data_ = nullptr;
            size_t m_stride_ = 0;
            
            std::string m_name_;
        };
    } // OpenGL::

    using cUnsafe_Buffer = OpenGL::cUnsafe_Buffer;
} // sk::Graphics::
