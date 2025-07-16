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
        virtual ~iWindow() = default;
        virtual void* create_context() = 0;
        virtual bool SetVisibility( bool _visible ) const = 0;
    };

    extern iWindow* create_window( const std::string& _name, const cVector2u& _size = {} );
} // sk::Platform
