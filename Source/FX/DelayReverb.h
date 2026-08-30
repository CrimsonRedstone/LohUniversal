#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>
#include <cmath>

/**
    Tape delay + Accutronics-style 2-spring reverb.

    The old 4-comb FDN rang like a metal plate. A spring is two slightly
    detuned delay lines with a cascade of allpasses (dispersion / "twang"),
    a band-pass (the spring doesn't do sub or air), and heavy HF damping
    so the tail is a drip, not a ping.
*/
class DelayReverb
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate > 1.0 ? spec.sampleRate : 44100.0;
        const int delayMax = juce::jmax(256, static_cast<int>(sampleRate * 1.2) + 8);
        int pow2 = 1;
        while (pow2 < delayMax)
            pow2 <<= 1;
        delayLineL.assign(static_cast<size_t>(pow2), 0.0f);
        delayLineR.assign(static_cast<size_t>(pow2), 0.0f);
        delayMask = pow2 - 1;
        delayWrite = 0;

        // Two springs, ~38 ms and ~44 ms at size=1, plus a short 11 ms chirp tank.
        const int s1 = juce::jmax(32, static_cast<int>(sampleRate * 0.038));
        const int s2 = juce::jmax(32, static_cast<int>(sampleRate * 0.044));
        const int s3 = juce::jmax(16, static_cast<int>(sampleRate * 0.011));
        springA.assign(static_cast<size_t>(s1), 0.0f);
        springB.assign(static_cast<size_t>(s2), 0.0f);
        chirp.assign(static_cast<size_t>(s3), 0.0f);
        posA = posB = posC = 0;

        const float sr = static_cast<float>(sampleRate);
        bpHp = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 320.0f / sr);
        bpLp = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 3200.0f / sr);
        dampC = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 1800.0f / sr);

        reset();
    }

    void reset() noexcept
    {
        std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
        std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
        delayWrite = 0;
        delayLpL = delayLpR = 0.0f;
        std::fill(springA.begin(), springA.end(), 0.0f);
        std::fill(springB.begin(), springB.end(), 0.0f);
        std::fill(chirp.begin(), chirp.end(), 0.0f);
        posA = posB = posC = 0;
        apL.fill(0.0f);
        apR.fill(0.0f);
        hpL = hpR = lpL = lpR = dampStateL = dampStateR = 0.0f;
        flutter = 0.0f;
    }

    void process(juce::AudioBuffer<float>& buffer,
                 float delayMix, float delayTimeMs, float delayFeedback,
                 float reverbMix, float reverbSize) noexcept
    {
        const int nCh = juce::jmin(2, buffer.getNumChannels());
        const int n = buffer.getNumSamples();
        const float dMix = juce::jlimit(0.0f, 1.0f, delayMix);
        const float rMix = juce::jlimit(0.0f, 1.0f, reverbMix);
        if (dMix <= 0.0001f && rMix <= 0.0001f)
            return;

        if (delayMask <= 0 || delayLineL.empty())
            return;

        const float delaySamp = juce::jlimit(8.0f,
            static_cast<float>(delayMask - 4),
            delayTimeMs * 0.001f * static_cast<float>(sampleRate));
        const float fb = juce::jlimit(0.0f, 0.88f, delayFeedback);
        const float size = juce::jlimit(0.05f, 1.0f, reverbSize);
        // Spring decay is short-to-medium; size mostly changes wetness of the tank, not a hall.
        const float decay = 0.42f + 0.45f * size;

        float* L = buffer.getWritePointer(0);
        float* R = nCh > 1 ? buffer.getWritePointer(1) : nullptr;

        const int nA = static_cast<int>(springA.size());
        const int nB = static_cast<int>(springB.size());
        const int nC = static_cast<int>(chirp.size());
        if (nA < 8 || nB < 8 || nC < 8)
            return;

        for (int i = 0; i < n; ++i)
        {
            const float inL = L[i];
            const float inR = R ? R[i] : inL;

            float delL = 0.0f, delR = 0.0f;
            if (dMix > 0.0001f)
            {
                const float read = static_cast<float>(delayWrite) - delaySamp;
                const int i0 = static_cast<int>(std::floor(read));
                const float frac = read - static_cast<float>(i0);
                const int i1 = i0 + 1;
                delL = delayLineL[static_cast<size_t>(i0 & delayMask)] * (1.0f - frac)
                     + delayLineL[static_cast<size_t>(i1 & delayMask)] * frac;
                delR = delayLineR[static_cast<size_t>(i0 & delayMask)] * (1.0f - frac)
                     + delayLineR[static_cast<size_t>(i1 & delayMask)] * frac;

                delayLpL += 0.22f * (delL - delayLpL);
                delayLpR += 0.22f * (delR - delayLpR);

                delayLineL[static_cast<size_t>(delayWrite)] = inL + delayLpR * fb;
                delayLineR[static_cast<size_t>(delayWrite)] = inR + delayLpL * fb;
            }
            else
            {
                delayLineL[static_cast<size_t>(delayWrite)] = inL;
                delayLineR[static_cast<size_t>(delayWrite)] = inR;
            }

            float revL = 0.0f, revR = 0.0f;
            if (rMix > 0.0001f)
            {
                flutter += 0.0007f;
                if (flutter > 1.0f) flutter -= 1.0f;
                const float wiggle = 0.015f * std::sin(juce::MathConstants<float>::twoPi * flutter);

                float xL = inL + delL * 0.12f;
                float xR = inR + delR * 0.12f;

                // Band-pass the send: springs don't do rumble or ice.
                hpL += bpHp * (xL - hpL); xL -= hpL;
                hpR += bpHp * (xR - hpR); xR -= hpR;
                lpL += bpLp * (xL - lpL); xL = lpL;
                lpR += bpLp * (xR - lpR); xR = lpR;

                // 4-stage dispersive allpass (the twang / chirp).
                xL = allpass4(xL, apL, 0.35f + wiggle);
                xR = allpass4(xR, apR, 0.38f - wiggle);

                float yA = springA[static_cast<size_t>(posA)];
                float yB = springB[static_cast<size_t>(posB)];
                float yC = chirp[static_cast<size_t>(posC)];

                dampStateL += dampC * (yA - dampStateL);
                dampStateR += dampC * (yB - dampStateR);
                yA = dampStateL;
                yB = dampStateR;

                springA[static_cast<size_t>(posA)] = xL + (yB * 0.65f + yC * 0.25f) * decay;
                springB[static_cast<size_t>(posB)] = xR + (yA * 0.65f + yC * 0.22f) * decay;
                chirp[static_cast<size_t>(posC)]   = 0.5f * (xL + xR) - yC * 0.35f;

                posA = (posA + 1) % nA;
                posB = (posB + 1) % nB;
                posC = (posC + 1) % nC;

                // Cross-mix so it's stereo-ish but still "one tank".
                revL = (yA * 0.85f + yB * 0.35f + yC * 0.20f) * 1.15f;
                revR = (yB * 0.85f + yA * 0.35f + yC * 0.20f) * 1.15f;

                if (! std::isfinite(revL) || ! std::isfinite(revR))
                {
                    revL = revR = 0.0f;
                    std::fill(springA.begin(), springA.end(), 0.0f);
                    std::fill(springB.begin(), springB.end(), 0.0f);
                    std::fill(chirp.begin(), chirp.end(), 0.0f);
                    apL.fill(0.0f);
                    apR.fill(0.0f);
                }
            }

            float outL = inL + delL * dMix + revL * rMix;
            float outR = inR + delR * dMix + revR * rMix;
            if (! std::isfinite(outL)) outL = 0.0f;
            if (! std::isfinite(outR)) outR = 0.0f;
            L[i] = outL;
            if (R) R[i] = outR;

            delayWrite = (delayWrite + 1) & delayMask;
        }
    }

private:
    static float allpass4(float x, std::array<float, 4>& ap, float a) noexcept
    {
        a = juce::jlimit(-0.7f, 0.7f, a);
        for (int s = 0; s < 4; ++s)
        {
            const float y = -a * x + ap[static_cast<size_t>(s)];
            ap[static_cast<size_t>(s)] = y * a + x;
            x = y;
        }
        return x;
    }

    double sampleRate = 44100.0;
    std::vector<float> delayLineL, delayLineR;
    int delayMask = 0;
    int delayWrite = 0;
    float delayLpL = 0.0f, delayLpR = 0.0f;

    std::vector<float> springA, springB, chirp;
    int posA = 0, posB = 0, posC = 0;
    std::array<float, 4> apL {}, apR {};
    float hpL = 0, hpR = 0, lpL = 0, lpR = 0, dampStateL = 0, dampStateR = 0;
    float bpHp = 0.05f, bpLp = 0.3f, dampC = 0.2f;
    float flutter = 0.0f;
};
