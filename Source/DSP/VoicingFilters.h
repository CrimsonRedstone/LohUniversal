#pragma once

#include <juce_dsp/juce_dsp.h>
#include <cmath>

/**
    Continental flute / reed voicing plus the Jennings integrator path.

    Flute : 12 dB/oct LPF around 900 Hz (2-pole).
    Reed  : 6 dB/oct HPF around 350 Hz into a tanh diode clipper.
    Jennings: leaky integration of the bus (square → saw) mixed in J70 mode.
*/
class VoicingFilters
{
public:
    void prepare(double sampleRate, int samplesPerBlock) noexcept;
    void reset() noexcept;

    void setFluteCutoff(float hz) noexcept;
    void setReedCutoff(float hz) noexcept;
    void setEngineMode(int mode) noexcept;

    /** Process a mono bus in place. */
    void process(float* data, int numSamples, float fluteAmount, float reedAmount) noexcept;

private:
    struct OnePole
    {
        float z = 0.0f;
        float a = 0.0f;

        void setLP(double sr, float hz) noexcept
        {
            const float x = std::exp(-2.0f * juce::MathConstants<float>::pi * hz / static_cast<float>(sr));
            a = 1.0f - x;
        }

        void setHP(double sr, float hz) noexcept
        {
            const float x = std::exp(-2.0f * juce::MathConstants<float>::pi * hz / static_cast<float>(sr));
            a = x;
        }

        float lp(float in) noexcept
        {
            z += a * (in - z);
            return z;
        }

        float hp(float in) noexcept
        {
            z = a * (z + in - prevIn);
            prevIn = in;
            return z;
        }

        float prevIn = 0.0f;
    };

    double sampleRate = 44100.0;
    int engineMode = 0;
    float fluteHz = 900.0f;
    float reedHz = 350.0f;

    OnePole flute1, flute2;
    OnePole reedHp;
    OnePole aa1, aa2;
    float jenningsState = 0.0f;
    float jenningsLeak = 0.995f;
    float aaCoeff = 0.4f;
};
