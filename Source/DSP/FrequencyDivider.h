#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>

/**
    Binary frequency divider with PolyBLEP anti-aliasing.

    The 12 master oscillators run so that **bit 31 of the phase word is C0**
    (MIDI 12). Each step *down* the bit index doubles frequency:

        bit 31 = C0    bit 30 = C1    …    bit 25 = C6    …    bit 22 = C9

    This is the only direction that works: in a phase accumulator the MSB is
    the *slowest* square. The previous mapping (MSB = C6, lower bits = lower
    octaves) was inverted — low keys read faster bits and screamed an octave
    (or four) too high.
*/
class FrequencyDivider
{
public:
    static inline float polyBLEP(float t, float dt) noexcept
    {
        if (dt <= 0.0f)
            return 0.0f;

        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0f;
        }

        if (t > 1.0f - dt)
        {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        }

        return 0.0f;
    }

    /** PolyBLEP square from a 32-bit phase, using bit `bit` as the cycle MSB. */
    static inline float squareAtBit(uint32_t phase, uint32_t increment, int bit) noexcept
    {
        bit = std::clamp(bit, 0, 31);
        const int shift = 31 - bit;
        const uint32_t shiftedPhase = phase << shift;

        const double dtD = static_cast<double>(increment) * std::ldexp(1.0, shift - 32);
        const float dt = static_cast<float>(std::min(0.45, std::max(dtD, 1.0e-9)));

        const float t = static_cast<float>(shiftedPhase) * (1.0f / 4294967296.0f);
        float s = (shiftedPhase & 0x80000000u) ? 1.0f : -1.0f;
        s += polyBLEP(t, dt);

        float tHalf = t + 0.5f;
        if (tHalf >= 1.0f)
            tHalf -= 1.0f;
        s -= polyBLEP(tHalf, dt);
        return s;
    }

    /**
        Square wave for an absolute MIDI note, sourced from the matching
        pitch-class master oscillator. Silent outside C0–C9 (the 16'→1'
        range of a 49-key combo organ).
    */
    static inline float squareForMidi(int midiNote,
                                      const uint32_t* phases,
                                      const uint32_t* increments) noexcept
    {
        if (midiNote < 12 || midiNote > 120)
            return 0.0f;

        const int pc = ((midiNote % 12) + 12) % 12;
        // MIDI 12 (C0) → 0 octaves above the MSB, MIDI 84 (C6) → 6, MIDI 120 (C9) → 9.
        const int octavesAboveC0 = (midiNote / 12) - 1;
        const int bit = 31 - octavesAboveC0;
        if (bit < 0)
            return 0.0f;
        return squareAtBit(phases[pc], increments[pc], bit);
    }

    /**
        Leaky-integrator saw derived from the square (Jennings J-series voicing).
        `state` is a persistent per-(pitchClass, octave-bucket) integrator.
    */
    static inline float integrateToSaw(float square, float& state, float leak) noexcept
    {
        state = state * leak + square * (1.0f - leak);
        return state * 2.4f;
    }
};
