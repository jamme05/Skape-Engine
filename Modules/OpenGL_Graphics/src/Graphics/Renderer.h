/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Singleton.h>

namespace sk::Graphics
{
    class cRenderer final : public cSingleton< cRenderer >
    {
    public:
        cRenderer();
    };
} // sk::Graphics
