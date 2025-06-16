/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Mesh_Helper.h"

#include <fastgltf/types.hpp>

namespace sk::Graphics::Helpers
{
    bool cMesh_Helper::ParseGltfPrimitiveMesh(
        const Assets::Mesh::ptr_t& _mesh, const fastgltf::Asset& _asset,
        const fastgltf::Primitive& _accessor )
    {
        // TODO: Get mesh functional with vulkan and then build the parser. Multi threaded?
        return false;
    } // ParseGltfAsset
} // sk::Graphics::Helpers