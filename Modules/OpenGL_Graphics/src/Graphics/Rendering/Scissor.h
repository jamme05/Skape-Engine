/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

namespace sk::Graphics
{
    // TODO: Create base in engine for the scissor.
    struct sScissor
    {
        int x;
        int y;
        size_t width;
        size_t height;
    };
} // Graphics
