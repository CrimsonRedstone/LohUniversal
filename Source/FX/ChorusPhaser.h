#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>
#include <vector>

/**
    Stereo chorus (modulated delay) and an 8-stage analogue phaser.
    Phaser: 80 Hz–1.8 kHz sweep, feedback, 8 allpass stages — clearly audible at mid mix.
*/
class ChorusPhaser
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate > 1.0 ? spec.sampleRate : 44100.0;
        int maxDelay = juce::jmax(64, static_cast<int>(sampleRate * 0.040) + 8);
        int pow2 = 1;
        while (pow2 < maxDelay)
            pow2 <<= 1;
        for (auto& d : chorusDelay)
            d.assign(static_cast<size_t>(pow2), 0.0f);
        dMask = pow2 - 1;
        writePos = 0;
        reset();
    }

    void reset() noexcept
    {
        for (auto& d : chorusDelay)
            std::fill(d.begin(), d.end(), 0.0f);
        writePos = 0;
        chorusPhase = 0.0f;
        phaserPhase = 0.0f;
        for (auto& a : allpass)
            a.fill(0.0f);
        phaserFb.fill(0.0f);
    }

    void process(juce::AudioBuffer<float>& buffer, float chorusMix, float phaserMix) noexcept
    {
        const int nCh = juce::jmin(2, buffer.getNumChannels());
        const int n = buffer.getNumSamples();
        const float cMix = juce::jlimit(0.0f, 1.0f, chorusMix);
        const float pMix = juce::jlimit(0.0f, 1.0f, phaserMix);

        if (cMix <= 0.0001f && pMix <= 0.0001f)
            return;

        if (dMask <= 0 || chorusDelay[0].empty())
            return;

        const float sr = static_cast<float>(sampleRate);
        const float chorusRate = 0.85f / sr;
        const float phaserRate = 0.35f / sr;               // slow sweep, more "through the amp"
        const float chorusBase = sr * 0.012f;
        const float chorusMod  = sr * 0.006f;
        const float ny = sr * 0.42f;

        for (int i = 0; i < n; ++i)
        {
            chorusPhase += chorusRate;
            if (chorusPhase >= 1.0f) chorusPhase -= 1.0f;
            phaserPhase += phaserRate;
            if (phaserPhase >= 1.0f) phaserPhase -= 1.0f;

            const float cLfoL = std::sin(juce::MathConstants<float>::twoPi * chorusPhase);
            const float cLfoR = std::sin(juce::MathConstants<float>::twoPi * chorusPhase + 1.7f);
            const float pLfo  = 0.5f + 0.5f * std::sin(juce::MathConstants<float>::twoPi * phaserPhase);

            for (int ch = 0; ch < nCh; ++ch)
            {
                float* data = buffer.getWritePointer(ch);
                const float dry = data[i];
                float y = dry;

                if (cMix > 0.0001f)
                {
                    const float lfo = (ch == 0) ? cLfoL : cLfoR;
                    const float delaySamp = chorusBase + chorusMod * lfo;
                    const float read = static_cast<float>(writePos) - delaySamp;
                    const int i0 = static_cast<int>(std::floor(read));
                    const float frac = read - static_cast<float>(i0);
                    auto& line = chorusDelay[static_cast<size_t>(ch)];
                    const float a = line[static_cast<size_t>(i0 & dMask)];
                    const float b = line[static_cast<size_t>((i0 + 1) & dMask)];
                    const float delayed = a + (b - a) * frac;
                    y = dry + delayed * 0.85f;
                    line[static_cast<size_t>(writePos)] = dry + delayed * 0.18f;
                    y = dry + (y - dry) * cMix;
                }
                else
                {
                    chorusDelay[static_cast<size_t>(ch)][static_cast<size_t>(writePos)] = dry;
                }

                if (pMix > 0.0001f)
                {
                    // 8-stage allpass, slightly offset per channel.
                    const float fc = juce::jlimit(60.0f, ny, 90.0f * std::pow(18.0f, pLfo) + (ch == 0 ? 0.0f : 18.0f));
                    const float w = std::tan(juce::MathConstants<float>::pi * fc / sr);
                    const float a1 = (w - 1.0f) / (w + 1.0f);

                    float x = y + phaserFb[static_cast<size_t>(ch)] * 0.72f;
                    x = juce::jlimit(-4.0f, 4.0f, x);
                    auto& ap = allpass[static_cast<size_t>(ch)];
                    for (int s = 0; s < 8; ++s)
                    {
                        const float z = -x * a1 + ap[static_cast<size_t>(s)];
                        ap[static_cast<size_t>(s)] = z * a1 + x;
                        x = z;
                    }
                    if (! std::isfinite(x))
                    {
                        x = 0.0f;
                        ap.fill(0.0f);
                        phaserFb[static_cast<size_t>(ch)] = 0.0f;
                    }
                    phaserFb[static_cast<size_t>(ch)] = juce::jlimit(-1.5f, 1.5f, x);
                    // Classic phaser: dry + inverted allpass. Mix 0.5 = deepest notches.
                    const float staged = 0.5f * (y - x);
                    const float depth = 0.35f + 0.65f * pMix; // already obvious at 30%
                    y = y + (staged - y) * depth;
                }

                if (! std::isfinite(y))
                    y = dry;
                data[i] = y;
            }

            writePos = (writePos + 1) & dMask;
        }
    }

private:
    double sampleRate = 44100.0;
    std::array<std::vector<float>, 2> chorusDelay;
    int dMask = 0;
    int writePos = 0;
    float chorusPhase = 0.0f;
    float phaserPhase = 0.0f;
    std::array<std::array<float, 8>, 2> allpass {};
    std::array<float, 2> phaserFb {};
};
