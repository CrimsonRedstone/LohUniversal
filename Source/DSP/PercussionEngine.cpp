#include "PercussionEngine.h"

void PercussionEngine::prepare(double sr) noexcept
{
    sampleRate = juce::jmax(8000.0, sr);
    reset();
    setDecayMs(decayMs);
}

void PercussionEngine::reset() noexcept
{
    env = 0.0f;
}

void PercussionEngine::setDecayMs(float ms) noexcept
{
    decayMs = juce::jlimit(20.0f, 1500.0f, ms);
    const float seconds = decayMs * 0.001f;
    // Envelope falls to -60 dB in `seconds`.
    coeff = std::exp(std::log(0.001f) / (seconds * static_cast<float>(sampleRate)));
}

void PercussionEngine::trigger() noexcept
{
    if (enabled)
        env = 1.0f;
}

float PercussionEngine::process(float fourthFootSample) noexcept
{
    if (! enabled && env <= 0.00001f)
        return 0.0f;

    const float level = soft ? 0.35f : 0.85f;
    const float out = fourthFootSample * env * level;
    env *= coeff;
    if (env < 0.00001f)
        env = 0.0f;
    return out;
}
