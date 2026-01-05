
#pragma once

#include <cstdint>

namespace sk::Assets
{
    enum class eEventType : uint8_t
    {
        kUnload,
        kLoaded,
        kUpdated
    };
} // sk::
