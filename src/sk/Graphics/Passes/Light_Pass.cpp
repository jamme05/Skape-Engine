

#include "Light_Pass.h"

#include <sk/Assets/Management/Asset_Manager.h>
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
    constexpr auto atlas_initial_size = 1024;
    m_shadow_context_ = SK_SINGLE( Rendering::cRender_Context, 2, 1 );
    for( const auto& context : *m_shadow_context_ )
    {
        context->Bind( 0, sk::MakeShared< Rendering::cRender_Target >(
            cVector2u32{ atlas_initial_size }, Rendering::cRender_Target::eFormat::kR16F ) );

        context->Bind( sk::MakeShared< Rendering::cDepth_Target >(
            cVector2u32{ atlas_initial_size }, Rendering::cDepth_Target::eFormat::kD16F ) );
    }

    m_atlases_.resize( 3 );
    for( uint_fast32_t i = 1; i < m_atlases_.size(); i++ )
    {
        m_atlases_[ i ] = sk::MakeShared< Rendering::cRender_Target >(
            cVector2u32{ atlas_initial_size / Math::pow2< uint32_t >( i ) }, Rendering::cRender_Target::eFormat::kR16F );
    }

    auto& asset_manager = cAsset_Manager::get();

    m_shadow_material_ = asset_manager.CreateAsset< Assets::cMaterial >( "Shadow Material", "builtin/shadow_mat.skmat",
        Utils::cShader_Link{
            asset_manager.GetAssetByPath( "shaders/default.vert" ),
            asset_manager.GetAssetByPath( "shaders/shadow.frag" )
        }
    ).first;
}

bool cLight_Pass::Begin()
{
    auto& manager = Scene::cLight_Manager::get();

    if( !m_shadow_material_->IsReady() )
        return false;

    manager.Update();

    auto& frame_buffer = m_shadow_context_->GetBack();
    frame_buffer.Clear( Rendering::eClear::kAll );

    if( const auto& depth_target = *frame_buffer.GetDepthTarget(); depth_target.GetResolution() != manager.GetAtlasSize() )
    {
        const auto new_resolution = manager.GetAtlasSize();
        frame_buffer.Resize( new_resolution );

        for( uint_fast32_t i = 1; i < m_atlases_.size(); i++ )
        {
            auto& target = *m_atlases_[ i ];
            target.Resize( new_resolution / Math::pow2( i ) );
        }
    }

    for( auto& shadow_caster : manager.GetShadowCasters() )
    {
        if( shadow_caster.is_valid() )
            _shadowPass( *shadow_caster );
    }

    m_shadow_context_->End();

    return true;
}

void cLight_Pass::End()
{
    auto& front_target = m_atlases_[ 0 ] = m_shadow_context_->GetFront().GetRenderTarget( 0 );
    Utils::CopyRenderTarget( *front_target, *m_atlases_[ 1 ] );
    Utils::CopyRenderTarget( *front_target, *m_atlases_[ 2 ] );
}

void cLight_Pass::Destroy()
{
    SK_DELETE( m_shadow_context_ );
    m_shadow_context_ = nullptr;
}

auto cLight_Pass::GetShadowAtlas( const size_t _index ) const -> const Rendering::cRender_Target&
{
    return *m_atlases_[ _index ];
}

void cLight_Pass::_shadowPass( const Object::Components::cLightComponent& _light )
{
    // TODO: Make a shadow pass
    auto& manager = Scene::cLight_Manager::get();
    auto& layer_manager = Scene::cLayer_Manager::get();

    auto& frame_buffer = m_shadow_context_->GetBack();

    frame_buffer.Begin( _getViewport( _light ), _getScissor( _light ) );

    layer_manager.Lock();
    for( auto [ fst, lst ] = layer_manager.GetMeshesIn( std::numeric_limits< size_t >::max() ); fst != lst; ++fst )
    {
        if( !fst.IsValid() )
            continue;

        auto& mesh = *fst;

        if( mesh == nullptr || !mesh->IsReady() )
            continue;

        // TODO: Add a local render function that's more optimized
        const bool res = Utils::RenderMesh( _light.GetViewProjMatrix(), frame_buffer,
            *m_shadow_material_, mesh->GetTransform().GetWorld(), *mesh->GetMesh() );
        if( !res )
            SK_BREAK;
    }
    layer_manager.Unlock();
}

auto cLight_Pass::_getViewport( const Object::Components::cLightComponent& _light ) -> sViewport
{
    auto& [ atlas_start, atlas_end, _ ] = *_light.GetShadowCasterData();

    return sViewport{
        .x = static_cast< int >( atlas_start.x ),
        .y = static_cast< int >( atlas_start.y ),
        .width  = atlas_end.x - atlas_start.x,
        .height = atlas_end.y - atlas_start.y
    };
}

auto cLight_Pass::_getScissor( const Object::Components::cLightComponent& _light ) -> sScissor
{
    auto& [ atlas_start, atlas_end, _ ] = *_light.GetShadowCasterData();

    return sScissor{
        .x = static_cast< int >( atlas_start.x ),
        .y = static_cast< int >( atlas_start.y ),
        .width  = atlas_end.x - atlas_start.x,
        .height = atlas_end.y - atlas_start.y
    };
}
