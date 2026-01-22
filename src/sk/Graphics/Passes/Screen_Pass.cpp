

#include "Screen_Pass.h"

#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Graphics/Rendering/Scissor.h>
#include <sk/Graphics/Rendering/Viewport.h>
#include <sk/Graphics/Rendering/Window_Context.h>
#include <sk/Math/Vector2.h>
#include <sk/Platform/Window/Window_Base.h>

using namespace sk::Graphics::Passes;

cScreen_Pass::cScreen_Pass( Platform::iWindow* _window, const cShared_ptr< cAsset_Meta >& _screen_material )
: m_screen_vertex_buffer_( "Screen Vertex Buffer", Buffer::eType::kVertex, false, 6, cVector2f{} )
, m_window_( _window )
, m_material_meta_( _screen_material )
{
    m_screen_vertex_buffer_.AlignAs< cVector2f >();
}

void cScreen_Pass::Init()
{
    static constexpr cVector2f kVertexArray[] = {
        { 0, 0 }, { 0, 1 }, { 1, 0 },
        { 1, 0 }, { 0, 1 }, { 1, 1 }
    };
    
    std::copy_n( kVertexArray, 6, m_screen_vertex_buffer_.Data< cVector2f >() );
    
    m_material_ = m_material_meta_.Lock();
}

bool cScreen_Pass::Begin()
{
    if( !m_material_.IsLoaded() )
        return false;
    
    const auto  resolution = m_window_->GetResolution();
    auto viewport = sViewport{ .x = 0, .y = 0, .width = resolution.x, .height = resolution.y };
    auto scissor  = sScissor{  .x = 0, .y = 0, .width = resolution.x, .height = resolution.y };
    
    auto& frame_buffer = m_window_->GetWindowContext().GetBack();
    
    frame_buffer.Begin( viewport, scissor );
    frame_buffer.Clear( Rendering::eClear::kAll );
    
    m_material_meta_->LockAsset();
    
    m_material_->Update();
    bool res = false;
    if( frame_buffer.UseMaterial( *m_material_ ) )
    {
        frame_buffer.BindVertexBuffer( 0, &m_screen_vertex_buffer_ );

        res = frame_buffer.DrawAuto();
        if( !res )
            SK_BREAK;
    }
    
    
    m_material_meta_->UnlockAsset();
    
    frame_buffer.UnbindVertexBuffers();
    frame_buffer.ResetMaterial();
    frame_buffer.End();
    
    return res;
}

void cScreen_Pass::End()
{
    m_window_->SwapBuffers();
}

void cScreen_Pass::Destroy()
{
    m_material_ = nullptr;
}
