#pragma once

#include <juce_core/juce_core.h>
#include <array>
#include <cstdint>
#include "../Parameters/ParameterLayout.h"

/**
    Twelve phase-locked master oscillators.

    Increments are set so bit 31 of each phase word is C0 (MIDI 12). Frequency
    dividers read lower bits for higher octaves (C6 lives at bit 25). A=440 ET.

    Each oscillator is a 32-bit phase accumulator. Frequency dividers read
    individual bits of the same phase word, so every octave of a pitch class
    is perfectly phase-locked — the Continental divide-down architecture.
*/
class TopOctaveOscillator
{
public:
    void prepare(double sampleRate) noexcept;
    void reset() noexcept;

    /** Advance all 12 phases by one sample. Vibrato is a shared ratio. */
    void tick(float vibratoRatio, float driftAmount, int chassisSkin) noexcept;

    uint32_t getPhase(int pitchClass) const noexcept { return phase[static_cast<size_t>(pitchClass)]; }
    uint32_t getIncrement(int pitchClass) const noexcept { return increment[static_cast<size_t>(pitchClass)]; }

    const uint32_t* getPhases() const noexcept { return phase.data(); }
    const uint32_t* getIncrements() const noexcept { return increment.data(); }

    void setDetuneCents(int pitchClass, float cents) noexcept;
    void setJunkyardJump(int pitchClass, float cents) noexcept;
    void applyNoteOnJitter(int pitchClass, uint32_t seed) noexcept;

    double getSampleRate() const noexcept { return sampleRate; }

private:
    void recomputeIncrements() noexcept;
    uint32_t nextRandom() noexcept;

    double sampleRate = 44100.0;
    std::array<uint32_t, 12> phase {};
    std::array<uint32_t, 12> increment {};
    std::array<float, 12> detuneCents {};
    std::array<float, 12> junkyardOffsetCents {};
    std::array<float, 12> junkyardJumpCents {};
    std::array<float, 12> driftState {};
    std::array<float, 12> wowPhase {};
    std::array<float, 12> wowRate {};

    uint32_t rng = 0xA341316Cu;
    float twoPiOverSr = 0.0f;
};
