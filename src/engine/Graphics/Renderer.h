/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/DerivedSingleton.h>

// Won't the graphics context have to live within each window?
// Figure out a way to either pick a context depending on the active window,
// or move part of the renderer singleton to the window. Like Window.GetRenderer()


namespace sk::Graphics
{
    // Will be overriden by Module
    class cRenderer : public cBaseSingleton< cRenderer >
    {
    protected:
    };
    
    extern void InitRenderer();
} // sk::Graphics
