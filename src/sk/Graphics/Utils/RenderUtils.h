/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Math/Matrix4x4.h>

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
    
    bool RenderMesh( const Object::Components::cCameraComponent& _camera, Rendering::cFrame_Buffer &_frame_buffer, Assets::cMaterial &_material,
        const cMatrix4x4f &_world_matrix, Assets::cMesh &_mesh );
} // sk::Graphics::Utils
