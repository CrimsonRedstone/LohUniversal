#include "TopOctaveOscillator.h"
#include <cmath>

void TopOctaveOscillator::prepare(double sr) noexcept
{
    sampleRate = juce::jmax(8000.0, sr);
    twoPiOverSr = static_cast<float>(2.0 * juce::MathConstants<double>::pi / sampleRate);
    reset();
}

void TopOctaveOscillator::reset() noexcept
{
    phase.fill(0);
    increment.fill(0);
    detuneCents.fill(0.0f);
    junkyardJumpCents.fill(0.0f);
    driftState.fill(0.0f);

    // Static junkyard offsets are unique per oscillator, deterministic so a
    // session recalls the same "broken" chassis.
    constexpr float kJunkyardStatic[12] = {
        -27.4f,  18.2f, -8.6f,  31.0f, -33.7f,  12.5f,
         22.1f, -15.8f,  6.3f, -29.9f,  34.6f,  -4.2f
    };
    for (int i = 0; i < 12; ++i)
        junkyardOffsetCents[static_cast<size_t>(i)] = kJunkyardStatic[i];

    constexpr float kWowRates[12] = {
        0.23f, 0.41f, 0.67f, 0.89f, 1.12f, 1.37f,
        1.61f, 1.88f, 2.07f, 2.21f, 2.39f, 0.55f
    };
    for (int i = 0; i < 12; ++i)
    {
        wowRate[static_cast<size_t>(i)]  = kWowRates[i];
        wowPhase[static_cast<size_t>(i)] = static_cast<float>(i) * 0.47f;
    }

    rng = 0xA341316Cu;
    recomputeIncrements();
}

void TopOctaveOscillator::setDetuneCents(int pitchClass, float cents) noexcept
{
    if (juce::isPositiveAndBelow(pitchClass, 12))
        detuneCents[static_cast<size_t>(pitchClass)] = juce::jlimit(-50.0f, 50.0f, cents);
}

void TopOctaveOscillator::setJunkyardJump(int pitchClass, float cents) noexcept
{
    if (juce::isPositiveAndBelow(pitchClass, 12))
        junkyardJumpCents[static_cast<size_t>(pitchClass)] = cents;
}

void TopOctaveOscillator::applyNoteOnJitter(int pitchClass, uint32_t seed) noexcept
{
    if (! juce::isPositiveAndBelow(pitchClass, 12))
        return;

    rng ^= seed * 1664525u + 1013904223u;
    const float jump = (static_cast<float>(rng & 0xFFFFu) * (1.0f / 32768.0f) - 1.0f) * 18.0f;
    junkyardJumpCents[static_cast<size_t>(pitchClass)] = jump;
}

uint32_t TopOctaveOscillator::nextRandom() noexcept
{
    rng = rng * 1664525u + 1013904223u;
    return rng;
}

void TopOctaveOscillator::recomputeIncrements() noexcept
{
    const double scale = 4294967296.0 / sampleRate;
    for (int i = 0; i < 12; ++i)
    {
        const double c0Hz = kTopOctaveHz[i] / 64.0;
        const double incD = c0Hz * scale;
        increment[static_cast<size_t>(i)] = static_cast<uint32_t>(juce::jlimit(1.0, 4294967295.0, incD));
    }
}

void TopOctaveOscillator::tick(float vibratoRatio, float driftAmount, int chassisSkin) noexcept
{
    const bool junkyard = (chassisSkin == ChassisSkin::Junkyard);
    const float driftAmt = juce::jlimit(0.0f, 1.0f, driftAmount);

    // Drift random-walk: MICRO_DRIFT 0–1 maps to ±1.5 … ±5 cents.
    const float maxCents = 1.5f + 3.5f * driftAmt;
    const float walk = 0.0009f * (0.15f + driftAmt);

    const double scale = 4294967296.0 / sampleRate;

    for (int i = 0; i < 12; ++i)
    {
        const size_t n = static_cast<size_t>(i);

        const float noise = (static_cast<float>(nextRandom() & 0xFFFFu) * (1.0f / 32768.0f) - 1.0f);
        driftState[n] += noise * walk;
        driftState[n] *= 0.9994f;
        driftState[n] = juce::jlimit(-1.0f, 1.0f, driftState[n]);

        float cents = detuneCents[n] + driftState[n] * maxCents;

        if (junkyard)
        {
            wowPhase[n] += wowRate[n] * twoPiOverSr;
            if (wowPhase[n] > juce::MathConstants<float>::twoPi)
                wowPhase[n] -= juce::MathConstants<float>::twoPi;

            const float wow = 4.5f * std::sin(wowPhase[n]);
            cents += junkyardOffsetCents[n] + junkyardJumpCents[n] + wow;
            junkyardJumpCents[n] *= 0.9992f; // settle after the note-on kick
        }

        const float ratio = vibratoRatio * std::pow(2.0f, cents / 1200.0f);
        // MSB of the phase word is C0 = C6 / 64. Higher octaves use lower bits.
        const double c0Hz = kTopOctaveHz[i] / 64.0;
        const double incD = c0Hz * scale * static_cast<double>(ratio);
        increment[n] = static_cast<uint32_t>(juce::jlimit(1.0, 4294967295.0, incD));
        phase[n] += increment[n];
    }
}
