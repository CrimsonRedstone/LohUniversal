#include "KeyNoiseGenerator.h"
#include <cmath>

void KeyNoiseGenerator::prepare(double sr) noexcept
{
    sampleRate = juce::jmax(8000.0, sr);
    hpCoeff = std::exp(-2.0f * juce::MathConstants<float>::pi * 1800.0f / static_cast<float>(sampleRate));
    lpCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 6500.0f / static_cast<float>(sampleRate));
    reset();
}

void KeyNoiseGenerator::reset() noexcept
{
    voices.fill({});
    nextVoice = 0;
    rng = 0xC001D00Du;
}

uint32_t KeyNoiseGenerator::nextRandom() noexcept
{
    rng = rng * 1664525u + 1013904223u;
    return rng;
}

void KeyNoiseGenerator::fire(Burst& b, float amount, bool noteOn) noexcept
{
    const float ms = noteOn ? 6.5f : 4.0f;
    b.coeff = std::exp(std::log(0.001f) / (ms * 0.001f * static_cast<float>(sampleRate)));
    b.env = juce::jlimit(0.0f, 1.0f, amount) * (noteOn ? 1.0f : 0.45f);
    b.hp = 0.0f;
    b.lp = 0.0f;
    b.prev = 0.0f;
    b.active = true;
}

void KeyNoiseGenerator::triggerOn(int, float amount) noexcept
{
    if (amount <= 0.0001f)
        return;
    fire(voices[static_cast<size_t>(nextVoice)], amount, true);
    nextVoice = (nextVoice + 1) % static_cast<int>(voices.size());
}

void KeyNoiseGenerator::triggerOff(int, float amount) noexcept
{
    if (amount <= 0.0001f)
        return;
    fire(voices[static_cast<size_t>(nextVoice)], amount * 0.7f, false);
    nextVoice = (nextVoice + 1) % static_cast<int>(voices.size());
}

float KeyNoiseGenerator::process() noexcept
{
    float mix = 0.0f;

    for (auto& b : voices)
    {
        if (! b.active)
            continue;

        const float white = (static_cast<float>(nextRandom() & 0xFFFFu) * (1.0f / 32768.0f) - 1.0f);
        b.hp = hpCoeff * (b.hp + white - b.prev);
        b.prev = white;
        b.lp += lpCoeff * (b.hp - b.lp);

        mix += b.lp * b.env;
        b.env *= b.coeff;
        if (b.env < 0.00001f)
        {
            b.env = 0.0f;
            b.active = false;
        }
    }

    return mix * 0.55f;
}
