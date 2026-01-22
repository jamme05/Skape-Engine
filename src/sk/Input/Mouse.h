
#pragma once

#ifdef SK_PLATFORM_SUPPORTS_MOUSE
#include <sk/Input/Mouse_Include.h>
#else // SK_PLATFORM_SUPPORTS_KEYBOARD
#include <cstdint>
namespace sk::Input::Mouse
{
    enum eButton : uint8_t
    {
        kLeft,
        kRight,
        kMiddle,
    };
} // sk::Input::Mouse::
#endif // !SK_PLATFORM_SUPPORTS_MOUSE