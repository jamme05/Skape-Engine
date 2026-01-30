/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Math/Matrix4x4.h>

namespace sk::Object::Components
{
    class cCameraComponent;
} // sk::Object::Components::

namespace sk::Graphics::Rendering
{
    class cFrame_Buffer;
} // sk::Graphics::Rendering::

namespace sk::Assets
{
    class cMaterial;
    class cMesh;
} // sk::Assets::

namespace sk::Graphics::Utils
{
    void InitUtils();
    void ShutdownUtils();

    void CopyRenderTarget( const Rendering::cRender_Target& _source, const Rendering::cRender_Target& _destination );
    bool RenderMesh( const cMatrix4x4f &_camera_view_proj, Rendering::cFrame_Buffer &_frame_buffer, Assets::cMaterial &_material,
        const cMatrix4x4f &_world_matrix, Assets::cMesh &_mesh );
} // sk::Graphics::Utils
