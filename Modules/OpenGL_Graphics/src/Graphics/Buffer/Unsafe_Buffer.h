/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer_Base.h>

#include <glbinding/gl/gl.h>

#include <atomic>
#include <mutex>
#include <string>

namespace sk::Graphics
{
    namespace OpenGL
    {
        // This is designed for being accessed by a rendering thread ( which locks/unlocks )
        // and a secondary thread which writes.
        class cUnsafe_Buffer final : public iUnsafe_Buffer
        {
            static constexpr gl::GLenum kTypeConverter[]
            {
                // kConstant
                gl::GLenum::GL_UNIFORM_BUFFER, // https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
                // kUniform
                gl::GLenum::GL_UNIFORM_BUFFER,
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
            cUnsafe_Buffer( std::string _name, size_t _byte_size, Buffer::eType _type, bool _is_static );
            cUnsafe_Buffer( std::string _name, size_t _byte_size, gl::GLenum _type, bool _is_static );
            cUnsafe_Buffer( const cUnsafe_Buffer& _other );
            cUnsafe_Buffer( cUnsafe_Buffer&& _other ) noexcept;

            ~cUnsafe_Buffer() override;

            cUnsafe_Buffer& operator=( const cUnsafe_Buffer& _other );
            cUnsafe_Buffer& operator=( cUnsafe_Buffer&& _other ) noexcept;

            void   Create();
            void   Copy( const cUnsafe_Buffer& _other );
            void   Destroy() const;

            void   Read     ( void* _out, size_t _max_size = 0 ) const override;
            void   ReadRaw  ( void* _out, size_t _max_size = 0 ) const override;
            void   Update   ( const void* _data, size_t _size ) override;
            void   UpdateSeg( const void* _data, size_t _size, size_t _offset ) override;
            [[ nodiscard ]]
            size_t GetSize() const override { return m_size_; }
            size_t GetSafeSize() const override;
            void   Resize ( size_t _byte_size ) override;
            void   Lock   () override;
            void   Unlock () override;
            [[ nodiscard ]]
            bool IsLocked() const override { return m_is_locked_; }

            // DO NOT USE ANYWHERE EXCEPT FOR INTERNAL USAGE
            [[ nodiscard ]]
            auto get_buffer() const { return m_buffer_; }
        private:
            void create_backup( size_t _size = 0, bool _force_copy = false );

            // If the backup is in use.
            std::atomic_bool m_in_use_    = false;
            std::atomic_bool m_is_locked_ = false;
            bool m_is_static_;
            bool m_has_backup_ = false;

            gl::GLenum m_type_;
            gl::GLuint m_buffer_;
            size_t     m_size_;

            size_t m_backup_size_ = 0;
            void*  m_backup_data_ = nullptr;

            std::string m_name_;

            std::mutex m_write_mtx_;
        };
    } // OpenGL::

    using cUnsafe_Buffer = OpenGL::cUnsafe_Buffer;
} // sk::Graphics::
