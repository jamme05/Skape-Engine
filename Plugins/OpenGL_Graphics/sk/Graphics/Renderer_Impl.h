/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Graphics/Renderer.h>
#include <sk/Graphics/Buffer/Unsafe_Buffer.h>
#include <sk/Graphics/Utils/Shader_Link.h>

namespace sk::Assets
{
    class cMesh;
} // sk::Assets

namespace sk::Graphics
{
    namespace Rendering
    {
        class cRender_Context;
    } // Rendering

    class cGLRenderer final : public cRenderer::Derived< cGLRenderer >
    {
    public:
        struct sTask
        {
            std::atomic_bool*       stopper;
            std::function< void() > function;
        };
        
        cGLRenderer();
        ~cGLRenderer() override;
        
        static void AddGLTask( const std::function< void() >& _function, bool _wait = true );

        auto& GetFallbackVertexBuffer() const { return *m_fallback_vertex_buffer_; }
        
        void Update() override;
    private:
        void addGLTask( const std::function< void() >& _function, bool _wait );

        std::unique_ptr< cUnsafe_Buffer > m_fallback_vertex_buffer_;
        
        std::mutex           m_task_mtx_;
        std::vector< sTask > m_tasks_;
    };
} // sk::Graphics
