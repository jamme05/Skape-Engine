/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer_Base.h>

#include <glad/glad.h>
#include <gl/GL.h>

#include <atomic>
#include <string>

namespace sk::Graphics
{
    namespace OpenGL
    {
        class cUnsafe_Buffer final : public iUnsafe_Buffer
        {
            static constexpr GLenum kTypeConverter[]
            {
                GL_UNIFORM_BUFFER, // https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
                GL_ARRAY_BUFFER, // https://learnopengl.com/Getting-started/Hello-Triangle#12
                GL_SHADER_STORAGE_BUFFER, // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
            };
            static constexpr GLenum kAccessConverter[]
            {
                GL_NONE,
                GL_READ_ONLY,
                GL_WRITE_ONLY,
                GL_READ_WRITE
            };
        public:
            cUnsafe_Buffer( const std::string& _name, size_t _byte_size, eType _type, bool _is_static );

            void   Read  ( void* _out, size_t _max_size ) override;
            void   Update( void* _data, size_t _size, size_t _offset ) override;
            [[ nodiscard ]]
            size_t GetSize() const override { return m_size_; }
            void   Resize ( size_t _byte_size ) override;
            void   Lock   () override;
            void   Unlock () override;
            [[ nodiscard ]]
            bool IsLocked() const override { return m_is_locked_; }

            // DO NOT USE ANYWHERE EXCEPT FOR INTERNAL USAGE
            [[ nodiscard ]]
            auto get_buffer() const { return m_buffer_; }
        private:
            std::atomic_bool m_is_locked_ = false;
            bool   m_is_static_;

            GLenum m_type_;
            GLuint m_buffer_;

            size_t m_size_;
            void*  m_backup_data_ = nullptr;

            std::string m_name_;
        };
    } // OpenGL::

    using cUnsafe_Buffer = OpenGL::cUnsafe_Buffer;
} // sk::Graphics::
