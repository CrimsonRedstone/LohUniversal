#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

/** Five peaking bands, post-amp. Gains in dB. */
class GraphicEQ
{
public:
    static constexpr int kBands = 5;
    static constexpr float kHz[kBands] = { 80.0f, 250.0f, 800.0f, 2500.0f, 8000.0f };

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate > 1.0 ? spec.sampleRate : 44100.0;
        for (auto& b : bands)
            for (auto& f : b.filt)
                f.reset();
        lastGain.fill(1.0e9f);
        reset();
    }

    void reset() noexcept
    {
        for (auto& b : bands)
            for (auto& f : b.filt)
                f.reset();
    }

    void process(juce::AudioBuffer<float>& buffer, const float gainsDb[kBands]) noexcept
    {
        const int nCh = juce::jmin(2, buffer.getNumChannels());
        const int n = buffer.getNumSamples();
        if (nCh < 1 || n <= 0)
            return;

        for (int b = 0; b < kBands; ++b)
        {
            const float g = juce::jlimit(-12.0f, 12.0f, gainsDb[b]);
            if (std::abs(g - lastGain[(size_t) b]) > 0.05f)
            {
                lastGain[(size_t) b] = g;
                auto coef = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                    sampleRate, kHz[b], 0.85f, juce::Decibels::decibelsToGain(g));
                for (int ch = 0; ch < 2; ++ch)
                    bands[(size_t) b].filt[(size_t) ch].coefficients = coef;
            }
        }

        for (int ch = 0; ch < nCh; ++ch)
        {
            float* data = buffer.getWritePointer(ch);
            for (int i = 0; i < n; ++i)
            {
                float x = data[i];
                for (int b = 0; b < kBands; ++b)
                    x = bands[(size_t) b].filt[(size_t) ch].processSample(x);
                if (! std::isfinite(x))
                    x = 0.0f;
                data[i] = x;
            }
        }
    }

private:
    struct Band
    {
        std::array<juce::dsp::IIR::Filter<float>, 2> filt;
    };

    double sampleRate = 44100.0;
    std::array<Band, kBands> bands;
    std::array<float, kBands> lastGain { 1.0e9f, 1.0e9f, 1.0e9f, 1.0e9f, 1.0e9f };
};
