/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset_List.h>

#include <Misc/Singleton.h>

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

    class cRenderer final : public cSingleton< cRenderer >
    {
    public:
        cRenderer();

        bool Render( Rendering::cRender_Context& _render_context, const Assets::cMesh& _mesh );
    private:
    };

    Assets::cAsset_List loadGLSL( const std::filesystem::path& _path );
} // sk::Graphics
