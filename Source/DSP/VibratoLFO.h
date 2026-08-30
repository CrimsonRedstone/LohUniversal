#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

/**
    Parallel-T style vibrato LFO.

    A pure sine at 4–8.5 Hz with a small 3rd-harmonic bulge that mimics the
    Continental's parallel-T oscillator, used to scale every master increment
    (phase-locked vibrato across the whole divider tree).
*/
class VibratoLFO
{
public:
    void prepare(double sampleRate) noexcept;
    void reset() noexcept;

    void setRate(float hz) noexcept { rateHz = juce::jlimit(4.0f, 8.5f, hz); }
    void setDepth(float d) noexcept { depth = juce::jlimit(0.0f, 1.0f, d); }
    void setEnabled(bool e) noexcept { enabled = e; }

    /** Returns a frequency ratio (≈ 1 ± a few cents) for this sample. */
    float nextRatio() noexcept;

    float peek() const noexcept { return lastValue; }

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float rateHz = 6.5f;
    float depth = 0.35f;
    float lastValue = 0.0f;
    bool enabled = false;
};
