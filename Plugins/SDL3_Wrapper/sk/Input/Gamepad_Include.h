/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <SDL3/SDL_gamepad.h>

#include <cstdint>

namespace sk::Input::Gamepad
{
    // TODO: More buttons
    enum eFaceButton : uint8_t
    {
        kSouth = SDL_GAMEPAD_BUTTON_SOUTH,
        kEast  = SDL_GAMEPAD_BUTTON_EAST,
        kWest  = SDL_GAMEPAD_BUTTON_WEST,
        kNorth = SDL_GAMEPAD_BUTTON_NORTH,
    };

    enum eAnalogButton : uint8_t
    {
        kLeftStick  = SDL_GAMEPAD_BUTTON_LEFT_STICK,
        kRightStick = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
        kLStick     = kLeftStick,
        kRStick     = kRightStick,

        kLeftShoulder  = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
        kLeftTrigger   = kLeftShoulder,
        kRightShoulder = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
        kRightTrigger  = kRightShoulder,
        kLTrigger      = kLeftShoulder,
        kRTrigger      = kRightShoulder,
    };

    enum eDPad : uint8_t
    {
        kUp    = SDL_GAMEPAD_BUTTON_DPAD_UP,
        kDown  = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
        kLeft  = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
        kRight = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
    };

    enum eSystem : uint8_t
    {
        kBack  = SDL_GAMEPAD_BUTTON_BACK,
        kGuide = SDL_GAMEPAD_BUTTON_GUIDE,
        kStart = SDL_GAMEPAD_BUTTON_START,
    };
} // sk::Input::Gamepad