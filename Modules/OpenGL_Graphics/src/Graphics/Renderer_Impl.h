/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset_List.h>
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

        // TODO: Move the rendering function and make it use a more abstracted API
        bool Render( Rendering::cRender_Context& _render_context, const Assets::cMesh& _mesh );
    private:
    };

    static void loadGLSL( const std::filesystem::path& _path, Assets::cAsset_List& _asset_list, Assets::eAssetTask _task );
} // sk::Graphics
