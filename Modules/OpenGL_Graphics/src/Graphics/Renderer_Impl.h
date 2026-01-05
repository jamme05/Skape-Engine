/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Utils/Asset_List.h>
#include <Assets/Management/Asset_Manager.h>
#include <Graphics/Renderer.h>


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
        cGLRenderer();
        ~cGLRenderer();
    private:
    };
} // sk::Graphics
