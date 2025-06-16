/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Assets/Mesh.h"

namespace fastgltf
{
    struct Primitive;
    class Asset;
} // fastgltf

namespace sk::Graphics::Helpers
{
    class cMesh_Helper
    {
    public:
        static bool ParseGltfPrimitiveMesh( const Assets::Mesh::ptr_t& _mesh, const fastgltf::Asset& _asset, const fastgltf::Primitive& _accessor );
    };
} // sk::Graphics::Helpers::Mesh