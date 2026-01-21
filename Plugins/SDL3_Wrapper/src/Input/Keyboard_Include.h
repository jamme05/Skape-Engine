/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>

#include <SDL3/SDL_keycode.h>

namespace sk::Input::Keyboard
{
    enum eNumerical : uint8_t
    {
        k0 = SDLK_0,
        k1 = SDLK_1,
        k2 = SDLK_2,
        k3 = SDLK_3,
        k4 = SDLK_4,
        k5 = SDLK_5,
        k6 = SDLK_6,
        k7 = SDLK_7,
        k8 = SDLK_8,
        k9 = SDLK_9,
    };

    enum eCharacters : uint8_t
    {
        kA = SDLK_A,
        kB = SDLK_B,
        kC = SDLK_C,
        kD = SDLK_D,
        kE = SDLK_E,
        kF = SDLK_F,
        kG = SDLK_G,
        kH = SDLK_H,
        kI = SDLK_I,
        kJ = SDLK_J,
        kK = SDLK_K,
        kL = SDLK_L,
        kM = SDLK_M,
        kN = SDLK_N,
        kO = SDLK_O,
        kP = SDLK_P,
        kQ = SDLK_Q,
        kR = SDLK_R,
        kS = SDLK_S,
        kT = SDLK_T,
        kU = SDLK_U,
        kV = SDLK_V,
        kW = SDLK_W,
        kX = SDLK_X,
        kY = SDLK_Y,
        kZ = SDLK_Z,
    };

    enum eSpecial : uint8_t
    {
        // Signs:
        kSpace      = SDLK_SPACE,
        /* ! */
        kExclaim    = SDLK_EXCLAIM,
        // DBLAPOSTROPHE????
        /* " */
        kQuote      = SDLK_DBLAPOSTROPHE,
        /* ' */
        kApostrophe = SDLK_APOSTROPHE,
        /* # */
        kHash       = SDLK_HASH,
        /* $ */
        kDollar     = SDLK_DOLLAR,
        /* % */
        kPercent    = SDLK_PERCENT,
        /* & */
        kAmpersand  = SDLK_AMPERSAND,
        /* * */
        kAsterisk   = SDLK_ASTERISK,
        /* + */
        kPlus       = SDLK_PLUS,
        /* - */
        kMinus      = SDLK_MINUS,
        /* , */
        kComma      = SDLK_COMMA,
        /* . */
        kPeriod     = SDLK_PERIOD,
        /* / */
        kSlash      = SDLK_SLASH,
        /* : */
        kColon      = SDLK_COLON,
        /* ; */
        kSemiColon  = SDLK_SEMICOLON,
        /* ? */
        kQuestion   = SDLK_QUESTION,
        /* = */
        kEqual      = SDLK_EQUALS,
        /* @ */
        kAt         = SDLK_AT,
        // TODO: Use this everywhere else.
        /* \ */
        kBackslash  = SDLK_BACKSLASH,
        /* ^ */
        kCaret      = SDLK_CARET,
        /* _ */
        kUnderscore = SDLK_UNDERSCORE,
        /* ` */
        kGrave      = SDLK_GRAVE,
        /* ~ */
        kTilde      = SDLK_TILDE,
        kPlusMinus  = SDLK_PLUSMINUS,
        /* | */
        kPipe       = SDLK_PIPE,

        // Bodies:
        /* < */
        kLess         = SDLK_LESS,
        /* > */
        kGreater      = SDLK_GREATER,
        /* ( */
        kLeftParen    = SDLK_LEFTPAREN,
        /* ) */
        kRightParen   = SDLK_RIGHTPAREN,
        /* [ */
        kLeftBracket  = SDLK_LEFTBRACKET,
        /* ] */
        kRightBracket = SDLK_RIGHTBRACKET,
        /* { */
        kLeftBrace    = SDLK_LEFTBRACE,
        /* } */
        kRightBrace   = SDLK_RIGHTBRACE,
    };

    // TODO: Keypad handling.
    enum eDirection
    {
        kRight = SDLK_RIGHT,
        kLeft  = SDLK_LEFT,
        kDown  = SDLK_DOWN,
        kUp    = SDLK_UP,
    };

    enum eInteracts
    {
        // Standard:
        kReturn      = SDLK_RETURN,
        kEsc         = SDLK_ESCAPE,
        kEscape      = SDLK_ESCAPE,
        kBackspace   = SDLK_BACKSPACE,
        kTab         = SDLK_TAB,
        kDelete      = SDLK_DELETE,
        kCapsLock    = SDLK_CAPSLOCK,
        kEnter       = SDLK_EXECUTE,
        kPrintScreen = SDLK_PRINTSCREEN,
        kScrollLock  = SDLK_SCROLLLOCK,
        kPause       = SDLK_PAUSE,
        kInsert      = SDLK_INSERT,
        kHome        = SDLK_HOME,
        kPageUp      = SDLK_PAGEUP,
        kPageDown    = SDLK_PAGEDOWN,

        // Fs
        kF1  = SDLK_F1,
        kF2  = SDLK_F2,
        kF3  = SDLK_F3,
        kF4  = SDLK_F4,
        kF5  = SDLK_F5,
        kF6  = SDLK_F6,
        kF7  = SDLK_F7,
        kF8  = SDLK_F8,
        kF9  = SDLK_F9,
        kF10 = SDLK_F10,
        kF11 = SDLK_F11,
        kF12 = SDLK_F12,
    };
} // sk::Input::Keyboard
