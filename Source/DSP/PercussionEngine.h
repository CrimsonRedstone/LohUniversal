#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

/**
    Single-bus percussion: exponential decay of the 4' rank, retriggered on
    every upper-manual note-on (Continental / combo-organ style, not Hammond
    first-note-only).
*/
class PercussionEngine
{
public:
    void prepare(double sampleRate) noexcept;
    void reset() noexcept;

    void setDecayMs(float ms) noexcept;
    void setEnabled(bool e) noexcept { enabled = e; }
    void setSoft(bool s) noexcept { soft = s; }

    void trigger() noexcept;

    float process(float fourthFootSample) noexcept;

    float envelope() const noexcept { return env; }

private:
    double sampleRate = 44100.0;
    float env = 0.0f;
    float coeff = 0.999f;
    float decayMs = 280.0f;
    bool enabled = false;
    bool soft = false;
};
