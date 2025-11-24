/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Renderer_Impl.h"

#include <Platform/Platform_Base.h>

#include <Assets/Management/Asset_Manager.h>

#include <glbinding/glbinding.h>

namespace sk::Graphics
{
    cGLRenderer::cGLRenderer()
    {
        glbinding::initialize( &Platform::get_proc_address );

        cAsset_Manager::get().AddFileLoader( { "glsl" }, &loadGLSL );
    } // cRenderer

    bool cGLRenderer::Render( Rendering::cRender_Context& _render_context, const Assets::cMesh& _mesh )
    {
        // TODO: Mesh rendering.
    }

    Assets::cAsset_List loadGLSL( const std::filesystem::path& _path )
    {
        
    }
} // sk::Graphics
