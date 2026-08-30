#pragma once

#include <juce_core/juce_core.h>

/** JUCE String(const char*) is Latin-1. Always wrap UTF-8 literals with this. */
inline juce::String U8(const char* utf8) noexcept
{
    return juce::String(juce::CharPointer_UTF8(utf8));
}

// Hex-escape fragments are split so MSVC cannot parse \x93B6 as one out-of-range
// escape (error C7744). Never write \xNN immediately followed by a hex letter
// that belongs to the next ASCII character (e.g. \xB135 for "±35").

inline juce::String u8MidDot()    { return U8("\xc2\xb7"); }          // ·
inline juce::String u8EnDash()    { return U8("\xe2\x80\x93"); }      // –
inline juce::String u8Arrow()     { return U8("\xe2\x86\x92"); }      // →
inline juce::String u8PlusMinus() { return U8("\xc2\xb1"); }          // ±
inline juce::String u8Cent()      { return U8("\xc2\xa2"); }          // ¢
inline juce::String u8OneThird()  { return U8("\xe2\x85\x93"); }      // ⅓
inline juce::String u8TwoThirds() { return U8("\xe2\x85\x94"); }      // ⅔
inline juce::String u8OneFifth()  { return U8("\xe2\x85\x95"); }      // ⅕

/** "Лох" */
inline juce::String lohBrand() noexcept
{
    return U8("\xd0\x9b\xd0\xbe\xd1\x85");
}

inline juce::String lohBrandFull() noexcept
{
    return lohBrand() + " Universal";
}
