#include "VibratoLFO.h"

void VibratoLFO::prepare(double sr) noexcept
{
    sampleRate = juce::jmax(8000.0, sr);
    reset();
}

void VibratoLFO::reset() noexcept
{
    phase = 0.0f;
    lastValue = 0.0f;
}

float VibratoLFO::nextRatio() noexcept
{
    const float inc = rateHz / static_cast<float>(sampleRate);
    phase += inc;
    if (phase >= 1.0f)
        phase -= 1.0f;

    const float twoPi = juce::MathConstants<float>::twoPi;
    const float sine = std::sin(twoPi * phase);
    // Parallel-T character: a touch of 3rd harmonic, slightly asymmetric.
    const float third = std::sin(twoPi * phase * 3.0f);
    lastValue = sine * 0.92f + third * 0.08f;

    if (! enabled || depth <= 0.0001f)
        return 1.0f;

    // Full depth ≈ ±22 cents — audible, musical, not seasick.
    const float cents = lastValue * depth * 22.0f;
    return std::pow(2.0f, cents / 1200.0f);
}
