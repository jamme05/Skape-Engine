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
    auto& link = const_cast< cShader_Link& >( _material.GetShaderLink() );
    SK_ERR_IFN( link.IsReady(),
        "Error: Link isn't ready yet, make sure to only call this function once Material.IsReady() returns true." )
    
    const auto object_block = _material.GetBlock( kObjectBlock );
    const auto camera_block = _material.GetBlock( kCameraBlock );
    
    SK_BREAK_RET_IF( sk::Severity::kGraphics,
        object_block == nullptr, "Currently you NEED a block named \"Object\" for this utility function to work.", false )
    
    SK_BREAK_RET_IF( sk::Severity::kGraphics,
        camera_block == nullptr, "Currently you NEED a block named \"Camera\" for this utility function to work.", false )
    
    // Object uniforms
    object_block->SetUniform( kWorldUniform,        _world_matrix );
    object_block->SetUniform( kInverseWorldUniform, _world_matrix.inversed() );
    
    // Camera uniforms
    camera_block->SetUniform( kInverseWorldUniform, _camera.getViewProjInv() );
    
    _material.Update();
    
    link.Use();
    ApplyMaterial( _material );
    
    auto& attributes     = link.GetReflection()->GetAttributes();
    auto& vertex_buffers = _mesh.GetVertexBuffers();
    
    for( auto& attribute : attributes )
    {
        // TODO: Fix the binding inside the frame buffer.
        if( auto itr = vertex_buffers.find( attribute.name ); itr != vertex_buffers.end() )
            _frame_buffer.BindVertexBuffer( attribute.index, *itr->second );
    }
    
    // TODO: Draw function.
    _frame_buffer.DrawIndexAuto(  );
    
    return true;
}

// TODO: Find a place to move this function to. The logic isn't allowed here.
bool Utils::ApplyMaterial( const Assets::cMaterial& _material )
{
    SK_BREAK_RET_IF( sk::Severity::kGraphics, !_material.IsReady(),
        "Error: Material isn't ready yet.", false )
    
    for( auto& block : _material.GetBlocks() | std::views::values )
        gl::glBindBufferBase( gl::GL_UNIFORM_BUFFER, static_cast< gl::GLuint >( block.m_binding_ ), block.m_buffer_.get_buffer().buffer );
    
    return true;
}
