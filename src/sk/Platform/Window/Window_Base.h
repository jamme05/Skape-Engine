/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Math/Vector2.h>

#include <string>

#include "sk/Graphics/Surface.h"

namespace sk::Graphics::Rendering
{
    class cWindow_Context;
} // sk::Graphics::Rendering

// Will be overriden by Module.
namespace sk::Platform
{
    class iWindow : public Graphics::iSurface
    {
    public:
        iWindow() = default;
        iWindow( const iWindow&  ) = delete;
        iWindow( iWindow&& other ) = delete;
        iWindow& operator=( const iWindow& ) = delete;
        iWindow& operator=( iWindow&& other) = delete;

        ~iWindow() override = default;
        
        virtual void Init() = 0;
        
        [[ nodiscard ]]
        virtual bool GetVisibility() const = 0;
        [[ nodiscard ]]
        virtual bool SetVisibility( bool _visible ) const = 0;
        [[ nodiscard ]]
        virtual auto GetAspectRatio() const -> float = 0;
        virtual void SetMouseCapture( bool _capture ) = 0;

        virtual void UseContext() = 0;
    };

    extern iWindow* CreateWindow ( const std::string_view& _name, const cVector2u32& _size = {} );
    extern void     DestroyWindow( iWindow* _window );
    extern iWindow* GetMainWindow();
} // sk::Platform
