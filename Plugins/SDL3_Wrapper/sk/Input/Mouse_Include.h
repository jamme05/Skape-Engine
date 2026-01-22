

#pragma once

#include <SDL3/SDL_mouse.h>

#include <cstdint>

namespace sk::Input::Mouse
{
    enum eButton : uint8_t
    {
        kLeft   = SDL_BUTTON_LEFT,
        kRight  = SDL_BUTTON_RIGHT,
        kMiddle = SDL_BUTTON_MIDDLE,
    };
} // sk::Input::Mouse::
