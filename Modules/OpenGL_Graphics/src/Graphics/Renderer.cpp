/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Renderer.h"

#include <glbinding/glbinding.h>

#include <Platform/Platform_Base.h>

#include "Assets/Manager/Asset_Manager.h"
#include "Rendering/Render_Context.h"

namespace sk::Graphics
{
    cRenderer::cRenderer()
    {
        glbinding::initialize( &Platform::get_proc_address );

        cAssetManager::get().AddFileLoader( { "glsl" }, &loadGLSL );
    } // cRenderer

    bool cRenderer::Render( Rendering::cRender_Context& _render_context, const Assets::cMesh& _mesh )
    {
        // TODO: Mesh rendering.
    }

    Assets::cAsset_List loadGLSL( const std::filesystem::path& _path )
    {
        
    }
} // sk::Graphics
