/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "RenderUtils.h"

#include <Assets/Material.h>
#include <Graphics/Renderer.h>
#include <Scene/Components/CameraComponent.h>

#include "Assets/Mesh.h"
#include "Graphics/Rendering/Frame_Buffer.h"
#include "Graphics/Utils/Shader_Reflection.h"

using namespace sk::Graphics;

namespace
{
    cRenderer *renderer;
    
    using namespace sk;
    
    // Blocks
    constexpr cStringID kObjectBlock = "_Object";
    constexpr cStringID kCameraBlock = "_Camera";
    
    // Object Uniforms
    constexpr cStringID kWorldUniform        = "world";
    constexpr cStringID kInverseWorldUniform = "inverse_world";
    
    // Camera Uniforms
    constexpr cStringID kViewProjUniform = "view_proj";
} // ::

void Utils::InitUtils()
{
    renderer = cRenderer::getPtr();
}

void Utils::ShutdownUtils()
{
    renderer = nullptr;
}

bool Utils::RenderMesh( const Object::Components::cCameraComponent& _camera, Rendering::cFrame_Buffer& _frame_buffer,
    Assets::cMaterial& _material, const cMatrix4x4f& _world_matrix, Assets::cMesh& _mesh )
{
    const auto& link = const_cast< cShader_Link& >( _material.GetShaderLink() );
    SK_ERR_IFN( link.IsReady(),
        "Error: Link isn't ready yet, make sure to only call this function once Material.IsReady() returns true." )
    
    const auto object_block = _material.GetBlock( kObjectBlock );
    const auto camera_block = _material.GetBlock( kCameraBlock );
    
    SK_BREAK_RET_IF( sk::Severity::kGraphics,
        object_block == nullptr, "Currently you NEED a block named \"Object\" for this utility function to work.", false )
    
    SK_BREAK_RET_IF( sk::Severity::kGraphics,
        camera_block == nullptr, "Currently you NEED a block named \"Camera\" for this utility function to work.", false )
    
    _mesh.GetMeta()->LockAsset();
    _material.GetMeta()->LockAsset();
    
    // Object uniforms
    object_block->SetUniform( kWorldUniform,        _world_matrix );
    object_block->SetUniform( kInverseWorldUniform, _world_matrix.inversed() );
    
    // Camera uniforms
    camera_block->SetUniform( kViewProjUniform, _camera.getViewProjInv() );
    
    _material.Update();
    
    _frame_buffer.UseMaterial( _material );
    
    auto& attributes     = link.GetReflection()->GetAttributes();
    auto& vertex_buffers = _mesh.GetVertexBuffers();
    
    for( auto& attribute : attributes )
    {
        if( auto itr = vertex_buffers.find( attribute.name ); itr != vertex_buffers.end() )
            _frame_buffer.BindVertexBuffer( attribute.index, itr->second.get() );
        else
            _frame_buffer.BindVertexBuffer( attribute.index, nullptr );
    }
    
    _frame_buffer.BindIndexBuffer( *_mesh.GetIndexBuffer() );

    const bool res = _frame_buffer.DrawIndexed();
    if( !res )
        SK_BREAK;
    
    _frame_buffer.UnbindIndexBuffer();
    _frame_buffer.UnbindVertexBuffers();
    _frame_buffer.ResetMaterial();
    
    _mesh.GetMeta()->UnlockAsset();
    _material.GetMeta()->UnlockAsset();
    
    return res;
}
