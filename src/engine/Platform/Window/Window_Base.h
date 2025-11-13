/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Math/Vector2.h"

// Will be overriden by Module.
namespace sk::Platform
{
    class iWindow
    {
    public:
        iWindow( const iWindow& ) = delete;
        iWindow( iWindow&& other) = delete;
        iWindow& operator=( const iWindow& ) = delete;
        iWindow& operator=( iWindow&& other) = delete;
        
        virtual ~iWindow() = default;
        virtual void* create_context() = 0;
        [[ nodiscard ]]
        virtual bool GetVisibility() const = 0;
        [[ nodiscard ]]
        virtual bool SetVisibility( bool _visible ) const = 0;
        [[ nodiscard ]]
        virtual cVector2u32 GetResolution () const = 0;
        virtual float       GetAspectRatio() const = 0;
    };

    extern iWindow* CreateWindow ( const std::string& _name, const cVector2u32& _size = {} );
    extern iWindow* GetMainWindow();
} // sk::Platform
