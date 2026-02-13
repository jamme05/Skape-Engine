
#include "RenderSurface.h"

#include <sk/Graphics/Rendering/Depth_Target.h>
#include <sk/Graphics/Rendering/Frame_Buffer.h>
#include <sk/Graphics/Rendering/Render_Target.h>
#include <sk/Platform/Time.h>

using namespace sk::Graphics::Utils;

cRenderSurface::cRenderSurface( const cVector2u32 _resolution )
: m_resolution_( _resolution )
{
    m_render_context_ = std::make_unique< Rendering::cRender_Context >();
    for( const auto& frame_buffer : *m_render_context_ )
    {
        frame_buffer->Bind( sk::MakeShared< Rendering::cRender_Target >( m_resolution_, Rendering::cRender_Target::eFormat::kRGBA8 ) );
        frame_buffer->Bind( sk::MakeShared< Rendering::cDepth_Target >( m_resolution_, Rendering::cDepth_Target::eFormat::kD24FS8 ) );
    }
}

bool cRenderSurface::WasResizedThisFrame() const
{
    return m_resized_on_frame_ == Time::Frame;
}

auto cRenderSurface::GetRenderContext() const -> Rendering::cRender_Context&
{
    return *m_render_context_;
}

auto cRenderSurface::GetResolution() const -> cVector2u32
{
    return m_resolution_;
}

void cRenderSurface::SetResolution( const cVector2u32 _resolution, const bool _resize_context )
{
    if( m_resolution_ == _resolution )
        return;

    m_resized_on_frame_ = Time::Frame;
    m_resolution_       = _resolution;

    if( _resize_context )
        m_render_context_->Resize( m_resolution_ );
}

void cRenderSurface::SwapBuffers()
{
    m_render_context_->Swap();
}
