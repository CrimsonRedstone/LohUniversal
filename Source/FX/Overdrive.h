#pragma once

#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <array>

/**
    Guitar-amp style overdrive for a combo organ.
    Pre-emphasis -> biased soft clip (even harmonics) -> speaker LPF.
    Makeup gain keeps loudness roughly constant so the knob is TONE, not volume.
*/
class Overdrive
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate > 1.0 ? spec.sampleRate : 44100.0;
        reset();
    }

    void reset() noexcept
    {
        lpL = lpR = hpL = hpR = preL = preR = sagL = sagR = 0.0f;
        const float sr = static_cast<float>(sampleRate);
        lpCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 3600.0f / sr);
        hpCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 90.0f / sr);
        preCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 620.0f / sr);
    }

    void process(juce::AudioBuffer<float>& buffer, float amount) noexcept
    {
        const float drive = juce::jlimit(0.0f, 1.0f, amount);
        if (drive <= 0.0001f)
            return;

        // Input gain grows, makeup falls — net level stays in the same ballpark.
        const float gain   = juce::Decibels::decibelsToGain(2.0f + drive * 22.0f);
        const float makeup = juce::Decibels::decibelsToGain(-(1.0f + drive * 14.0f));
        const float mix    = 0.15f + 0.85f * drive;
        const float bias   = 0.18f * drive;          // even harmonics
        const float sagAmt = 0.35f * drive;

        const int nCh = juce::jmin(2, buffer.getNumChannels());
        const int n = buffer.getNumSamples();

        for (int ch = 0; ch < nCh; ++ch)
        {
            float* data = buffer.getWritePointer(ch);
            float& lp = (ch == 0 ? lpL : lpR);
            float& hp = (ch == 0 ? hpL : hpR);
            float& pre = (ch == 0 ? preL : preR);
            float& sag = (ch == 0 ? sagL : sagR);

            for (int i = 0; i < n; ++i)
            {
                const float dry = data[i];

                hp += hpCoeff * (dry - hp);
                float x = dry - hp;                         // HPF rumble

                pre += preCoeff * (x - pre);
                x = x + (x - pre) * (0.6f + 1.4f * drive);  // 720 Hz presence

                const float env = std::fabs(x);
                sag += 0.0008f * (env - sag);
                const float sagGain = 1.0f / (1.0f + sag * sagAmt * 8.0f);

                x = x * gain * sagGain + bias;

                // Asymmetric clip: harder on the negative half (tube-ish).
                const float pos = std::tanh(x);
                const float neg = std::tanh(x * 1.45f);
                x = (x >= 0.0f ? pos : neg);

                lp += lpCoeff * (x - lp);                   // speaker
                const float wet = lp * makeup * 1.15f;
                float y = dry * (1.0f - mix) + wet * mix;
                if (! std::isfinite(y))
                    y = 0.0f;
                data[i] = y;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    float lpL = 0, lpR = 0, hpL = 0, hpR = 0, preL = 0, preR = 0, sagL = 0, sagR = 0;
    float lpCoeff = 0.4f, hpCoeff = 0.01f, preCoeff = 0.1f;
};

/** Classic wah: resonant SVF bandpass, cutoff 350–2200 Hz, hard-clamped so it cannot blow up. */
class WahPedal
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate > 1.0 ? spec.sampleRate : 44100.0;
        reset();
    }

    void reset() noexcept
    {
        lowL = bandL = highL = 0.0f;
        lowR = bandR = highR = 0.0f;
    }

    void process(juce::AudioBuffer<float>& buffer, float position) noexcept
    {
        const float pos = juce::jlimit(0.0f, 1.0f, position);
        const float fc = 350.0f * std::pow(2200.0f / 350.0f, pos);
        const float ny = static_cast<float>(sampleRate) * 0.45f;
        const float f = juce::jlimit(0.01f, 0.85f,
            2.0f * std::sin(juce::MathConstants<float>::pi * juce::jmin(fc, ny) / static_cast<float>(sampleRate)));
        const float q = 0.18f;
        const int nCh = juce::jmin(2, buffer.getNumChannels());
        const int n = buffer.getNumSamples();

        for (int i = 0; i < n; ++i)
        {
            for (int ch = 0; ch < nCh; ++ch)
            {
                float& low  = (ch == 0 ? lowL  : lowR);
                float& band = (ch == 0 ? bandL : bandR);
                float& high = (ch == 0 ? highL : highR);
                float* data = buffer.getWritePointer(ch);
                const float in = data[i];
                low  += f * band;
                high  = in - low - q * band;
                band += f * high;
                if (! std::isfinite(low) || ! std::isfinite(band) || std::fabs(band) > 24.0f)
                {
                    low = band = high = 0.0f;
                    data[i] = in;
                    continue;
                }
                data[i] = band * 1.6f + in * 0.12f;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    float lowL = 0, bandL = 0, highL = 0;
    float lowR = 0, bandR = 0, highR = 0;
};

/** AC30-ish 1x12: low bump, dark mid, Vox-style cut. Drive/Bass/Treble/Cut are live. */
class AC30Cabinet
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate > 1.0 ? spec.sampleRate : 44100.0;
        reset();
        lastDrive = lastBass = lastTreble = lastCut = 1.0e9f;
    }

    void reset() noexcept
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            shelf[(size_t) ch].reset();
            midScoop[(size_t) ch].reset();
            presence[(size_t) ch].reset();
            highCut[(size_t) ch].reset();
            pre[(size_t) ch] = 0.0f;
        }
    }

    void process(juce::AudioBuffer<float>& buffer,
                 float drive, float bassDb, float trebleDb, float cut) noexcept
    {
        drive    = juce::jlimit(0.0f, 1.0f, drive);
        bassDb   = juce::jlimit(-12.0f, 12.0f, bassDb);
        trebleDb = juce::jlimit(-12.0f, 12.0f, trebleDb);
        cut      = juce::jlimit(0.0f, 1.0f, cut);

        if (std::abs(drive - lastDrive) > 0.01f
            || std::abs(bassDb - lastBass) > 0.15f
            || std::abs(trebleDb - lastTreble) > 0.15f
            || std::abs(cut - lastCut) > 0.01f)
        {
            lastDrive = drive;
            lastBass = bassDb;
            lastTreble = trebleDb;
            lastCut = cut;
            updateCoeffs(drive, bassDb, trebleDb, cut);
        }

        const float gain = juce::Decibels::decibelsToGain(drive * 10.0f);
        const int nCh = juce::jmin(2, buffer.getNumChannels());
        const int n = buffer.getNumSamples();
        for (int ch = 0; ch < nCh; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            auto& sh = shelf[(size_t) ch];
            auto& ms = midScoop[(size_t) ch];
            auto& pr = presence[(size_t) ch];
            auto& hc = highCut[(size_t) ch];
            float& p = pre[(size_t) ch];
            for (int i = 0; i < n; ++i)
            {
                float x = data[i] * (0.85f + gain * 0.35f);
                x = sh.processSample(x);
                x = ms.processSample(x);
                x = pr.processSample(x);
                x = hc.processSample(x);
                // Mild 12AX7 sag, darker than a guitar amp.
                p += 0.0007f * (std::fabs(x) - p);
                const float sag = 1.0f / (1.0f + p * drive * 4.0f);
                x = std::tanh(x * (1.05f + drive * 0.55f) * sag);
                if (! std::isfinite(x))
                    x = 0.0f;
                data[i] = x;
            }
        }
    }

private:
    void updateCoeffs(float drive, float bassDb, float trebleDb, float cut) noexcept
    {
        const float lpHz = juce::jmap(cut, 5000.0f, 2300.0f);
        auto bass = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, 115.0f, 0.7f, juce::Decibels::decibelsToGain(3.5f + bassDb));
        auto scoop = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, 500.0f, 0.7f, juce::Decibels::decibelsToGain(-4.0f - drive * 1.5f));
        auto treble = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, 2100.0f, 0.7f, juce::Decibels::decibelsToGain(trebleDb));
        auto lp = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, lpHz, 0.7f);
        for (int ch = 0; ch < 2; ++ch)
        {
            shelf[(size_t) ch].coefficients = bass;
            midScoop[(size_t) ch].coefficients = scoop;
            presence[(size_t) ch].coefficients = treble;
            highCut[(size_t) ch].coefficients = lp;
        }
    }

    double sampleRate = 44100.0;
    std::array<juce::dsp::IIR::Filter<float>, 2> shelf, midScoop, presence, highCut;
    std::array<float, 2> pre { 0.0f, 0.0f };
    float lastDrive = 1.0e9f, lastBass = 1.0e9f, lastTreble = 1.0e9f, lastCut = 1.0e9f;
};
