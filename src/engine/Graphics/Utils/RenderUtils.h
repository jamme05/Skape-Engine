/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Math/Matrix4x4.h>

namespace sk::Graphics::Rendering
{
    class cFrame_Buffer;
}

namespace sk::Assets
{
    class cShader;
    class cMesh;
}

namespace sk::Graphics::Utils
{
    bool RenderMesh( Rendering::cFrame_Buffer &_frame_buffer, Assets::cShader &_shader,
        const cMatrix4x4f &_world_matrix, Assets::cMesh &_mesh );
} // sk::Graphics::Utils
