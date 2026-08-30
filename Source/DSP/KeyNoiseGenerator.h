#pragma once

#include <juce_core/juce_core.h>
#include <array>
#include <cstdint>

/**
    Key click: a short filtered-noise burst on note-on and a quieter thunk
    on note-off, modelling Continental contact bounce and busbar scrape.
*/
class KeyNoiseGenerator
{
public:
    void prepare(double sampleRate) noexcept;
    void reset() noexcept;

    void triggerOn(int midiNote, float amount) noexcept;
    void triggerOff(int midiNote, float amount) noexcept;

    float process() noexcept;

private:
    struct Burst
    {
        float env = 0.0f;
        float coeff = 0.0f;
        float hp = 0.0f;
        float lp = 0.0f;
        float prev = 0.0f;
        bool active = false;
    };

    void fire(Burst& b, float amount, bool noteOn) noexcept;
    uint32_t nextRandom() noexcept;

    double sampleRate = 44100.0;
    std::array<Burst, 8> voices {};
    int nextVoice = 0;
    uint32_t rng = 0xC001D00Du;
    float hpCoeff = 0.0f;
    float lpCoeff = 0.0f;
};
