

#pragma once

#include <Graphics/Renderer.h>
#include <vulkan/vulkan_raii.hpp>

namespace sk::Graphics
{
    class cVKRenderer final : public cRenderer::Derived< cVKRenderer >
    {
    public:
        cVKRenderer();
        
    private:
        void createInstance();
        
        vk::raii::Context  m_context_;
        vk::raii::Instance m_instance_ = nullptr;
    };
} // sk::Graphics
