/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Math/Vector2.h>

namespace sk::Graphics::Rendering
{
    class cWindow_Context;
} // sk::Graphics::Rendering

// Will be overriden by Module.
namespace sk::Platform
{
    class iWindow
    {
    public:
        iWindow() = default;
        iWindow( const iWindow& ) = delete;
        iWindow( iWindow&& other) = delete;
        iWindow& operator=( const iWindow& ) = delete;
        iWindow& operator=( iWindow&& other) = delete;
        
        virtual ~iWindow() = default;
        
        virtual void Init() = 0;
        
        [[ nodiscard ]]
        virtual bool GetVisibility() const = 0;
        [[ nodiscard ]]
        virtual bool SetVisibility( bool _visible ) const = 0;
        [[ nodiscard ]]
        virtual cVector2u32 GetResolution () const = 0;
        virtual float       GetAspectRatio() const = 0;
        virtual void        SetMouseCapture( bool _capture ) = 0;
        
        virtual auto GetWindowContext() const -> Graphics::Rendering::cWindow_Context& = 0;
        
        virtual void SwapBuffers() = 0;
        
        virtual void PushContext() = 0;
        virtual void PopContext () = 0;
        
        virtual bool WasResizedThisFrame() const = 0;
    };

    extern iWindow* CreateWindow ( const std::string& _name, const cVector2u32& _size = {} );
    extern void     DestroyWindow( iWindow* _window );
    extern iWindow* GetMainWindow();
} // sk::Platform
