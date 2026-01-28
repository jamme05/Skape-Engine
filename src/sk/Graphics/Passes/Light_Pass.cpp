

#include "Light_Pass.h"

#include <sk/Graphics/Rendering/Depth_Target.h>
#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Graphics/Rendering/Render_Context.h>
#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Graphics/Rendering/Viewport.h>
#include <sk/Graphics/Utils/RenderUtils.h>
#include <sk/Scene/Components/MeshComponent.h>
#include <sk/Scene/Managers/Layer_Manager.h>
#include <sk/Scene/Managers/Light_Manager.h>


using namespace sk::Graphics::Passes;


void cLight_Pass::Init()
{
    m_shadow_context_ = SK_SINGLE( Rendering::cRender_Context );
    for( const auto& context : *m_shadow_context_ )
    {
        context->Bind( 0, sk::make_shared< Rendering::cRender_Target >(
            cVector2u32{ 1024 }, Rendering::cRender_Target::eFormat::kR16F ) );
        context->Bind( sk::make_shared< Rendering::cDepth_Target >(
            cVector2u32{ 1024 }, Rendering::cDepth_Target::eFormat::kD16F ) );
    }
}

bool cLight_Pass::Begin()
{
    auto& manager = Scene::cLight_Manager::get();

    manager.Update();

    for( auto& shadow_caster : manager.GetShadowCasters() )
    {
        if( shadow_caster.is_valid() )
            _shadowPass( *shadow_caster );
    }

    return true;
}

void cLight_Pass::End()
{
    
}

void cLight_Pass::Destroy()
{
    SK_FREE( m_shadow_context_ );
    m_shadow_context_ = nullptr;
}

void cLight_Pass::_shadowPass( const Object::Components::cLightComponent& _light )
{
    // TODO: Make a shadow pass
    auto& manager = Scene::cLight_Manager::get();
    const auto& layer_manager = Scene::cLayer_Manager::get();

    auto& frame_buffer = m_shadow_context_->GetBack();

    frame_buffer.Begin( _getViewport( _light ), _getScissor( _light ) );
    frame_buffer.Clear( Rendering::eClear::kAll );

    for( auto [ fst, lst ] = layer_manager.GetMeshesIn( std::numeric_limits< size_t >::max() ); fst != lst; ++fst )
    {
        if( !fst.IsValid() )
            continue;

        auto& mesh = *fst;

        if( !mesh->IsReady() )
            continue;

        // TODO: Add a local render function that's more optimized
        const bool res = Utils::RenderMesh( _light.GetViewProjMatrix(), frame_buffer,
            *mesh->GetMaterial(), mesh->GetTransform().GetWorld(), *mesh->GetMesh() );
        if( !res )
            SK_BREAK;
    }

}

auto cLight_Pass::_getViewport( const Object::Components::cLightComponent& _light ) -> sViewport
{
    auto atlas_size = Scene::cLight_Manager::get().GetAtlasSize();

    auto& [ atlas_start, atlas_end, _ ] = *_light.GetShadowCasterData();

    cVector2u32 viewport_start = atlas_start * atlas_size;
    cVector2u32 viewport_end   = atlas_end * atlas_size;

    return sViewport{
        .x = static_cast< int >( viewport_start.x ),
        .y = static_cast< int >( viewport_start.y ),
        .width  = viewport_end.x - viewport_start.x,
        .height = viewport_end.y - viewport_start.y
    };
}

auto cLight_Pass::_getScissor( const Object::Components::cLightComponent& _light ) -> sScissor
{
    auto atlas_size = Scene::cLight_Manager::get().GetAtlasSize();

    auto& [ atlas_start, atlas_end, _ ] = *_light.GetShadowCasterData();

    cVector2u32 viewport_start = atlas_start * atlas_size;
    cVector2u32 viewport_end   = atlas_end * atlas_size;

    return sScissor{
        .x = static_cast< int >( viewport_start.x ),
        .y = static_cast< int >( viewport_start.y ),
        .width  = viewport_end.x - viewport_start.x,
        .height = viewport_end.y - viewport_start.y
    };
}
