

#pragma once

#if SK_PLATFORM_SUPPORTS_GAMEPAD
#include <Input/Gamepad_Include.h>
#else // SK_PLATFORM_SUPPORTS_GAMEPAD
#include <cstdint>

namespace sk::Input::Gamepad
{
    // TODO: More buttons
    enum eFaceButton : uint8_t
    {
        kSouth,
        kEast,
        kWest,
        kNorth,
    };

    enum eAnalogButton : uint8_t
    {
        kLeftStick  = 0,
        kRightStick = 1,
        kLStick     = kLeftStick,
        kRStick     = kRightStick,

        kLeftShoulder  = 2,
        kLeftTrigger   = kLeftShoulder,
        kRightShoulder = 3,
        kRightTrigger  = kRightShoulder,
        kLTrigger      = kLeftShoulder,
        kRTrigger      = kRightShoulder,
    };

    enum eDPad : uint8_t
    {
        kUp,
        kDown,
        kLeft,
        kRight,
    };

    enum eSystem : uint8_t
    {
        kBack,
        kGuide,
        kStart,
    };
} // sk::Input
#endif // !SK_PLATFORM_SUPPORTS_GAMEPAD