/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Renderer.h"

#include <glbinding/glbinding.h>

#include <Platform/Platform_Base.h>

namespace sk::Graphics
{
    cRenderer::cRenderer()
    {
        glbinding::initialize( &Platform::getProcAddress );
    }
} // sk::Graphics
