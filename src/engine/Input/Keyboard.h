

#pragma once

#if SK_PLATFORM_SUPPORTS_KEYBOARD
#include <Input/Keyboard_Include.h>
#else // SK_PLATFORM_SUPPORTS_KEYBOARD
#include <cstdint>
namespace sk::Input::Keyboard
{
    enum eNumerical : uint8_t
    {
        k0,
        k1,
        k2,
        k3,
        k4,
        k5,
        k6,
        k7,
        k8,
        k9,
    };

    enum eCharacters : uint8_t
    {
        kA,
        kB,
        kC,
        kD,
        kE,
        kF,
        kG,
        kH,
        kI,
        kJ,
        kK,
        kL,
        kM,
        kN,
        kO,
        kP,
        kQ,
        kR,
        kS,
        kT,
        kU,
        kV,
        kW,
        kX,
        kY,
        kZ,
    };

    enum eSpecial : uint8_t
    {
        // Signs:
        kSpace,
        /* ! */
        kExclaim,
        /* " */
        kQuote,
        /* ' */
        kApostrophe,
        /* # */
        kHash,
        /* $ */
        kDollar,
        /* % */
        kPercent,
        /* & */
        kAmpersand,
        /* * */
        kAsterisk,
        /* + */
        kPlus,
        /* - */
        kMinus,
        /* , */
        kComma,
        /* . */
        kPeriod,
        /* / */
        kSlash,
        /* : */
        kColon,
        /* ; */
        kSemiColon,
        /* ? */
        kQuestion,
        /* = */
        kEqual,
        /* @ */
        kAt,
        /* \ */
        kBackslash,
        /* ^ */
        kCaret,
        /* _ */
        kUnderscore,
        /* ` */
        kGrave,
        /* ~ */
        kTilde,
        kPlusMinus,
        /* | */
        kPipe,

        // Bodies:
        /* < */
        kLess,
        /* > */
        kGreater,
        /* ( */
        kLeftParen,
        /* ) */
        kRightParen,
        /* [ */
        kLeftBracket,
        /* ] */
        kRightBracket,
        /* { */
        kLeftBrace,
        /* } */
        kRightBrace,
    };

    // TODO: Keypad handling.
    enum eDirection
    {
        kRight,
        kLeft,
        kDown,
        kUp,
    };

    enum eInteracts
    {
        // Standard:
        kReturn,
        kEsc,
        kEscape,
        kBackspace,
        kTab,
        kDelete,
        kCapsLock,
        kEnter,
        kPrintScreen,
        kScrollLock,
        kPause,
        kInsert,
        kHome,
        kPageUp,
        kPageDown,

        // Fs
        kF1,
        kF2,
        kF3,
        kF4,
        kF5,
        kF6,
        kF7,
        kF8,
        kF9,
        kF10,
        kF11,
        kF12,
    };
} // sk::Input::Keyboard
#endif // !SK_PLATFORM_SUPPORTS_KEYBOARD