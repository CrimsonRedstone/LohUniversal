#include "LeslieRotarySpeaker.h"
#include "../Parameters/ParameterLayout.h"
#include <cmath>

void LeslieRotarySpeaker::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    int maxDelay = juce::jmax(64, static_cast<int>(sampleRate * 0.012) + 16);
    int pow2 = 1;
    while (pow2 < maxDelay)
        pow2 <<= 1;

    hornDelayL.assign(static_cast<size_t>(pow2), 0.0f);
    hornDelayR.assign(static_cast<size_t>(pow2), 0.0f);
    drumDelayL.assign(static_cast<size_t>(pow2), 0.0f);
    drumDelayR.assign(static_cast<size_t>(pow2), 0.0f);
    delayMask = pow2 - 1;
    writePos = 0;

    // Crossover ~800 Hz (horn above, drum below).
    xoverCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 800.0f
                                 / static_cast<float>(sampleRate));

    // Horn accelerates faster than the drum (physical inertia).
    horn.accel = 2.8f;
    horn.decel = 1.6f;
    drum.accel = 0.85f;
    drum.decel = 0.45f;

    reset();
}

void LeslieRotarySpeaker::reset() noexcept
{
    horn.angle = 0.0f;
    drum.angle = 0.37f;
    horn.currentHz = 0.0f;
    drum.currentHz = 0.0f;
    std::fill(hornDelayL.begin(), hornDelayL.end(), 0.0f);
    std::fill(hornDelayR.begin(), hornDelayR.end(), 0.0f);
    std::fill(drumDelayL.begin(), drumDelayL.end(), 0.0f);
    std::fill(drumDelayR.begin(), drumDelayR.end(), 0.0f);
    writePos = 0;
    xoverLpL = xoverLpR = 0.0f;
    cabZ1L = cabZ2L = cabZ1R = cabZ2R = 0.0f;
}

void LeslieRotarySpeaker::setSpeed(int rotarySpeedEnum) noexcept
{
    speed = rotarySpeedEnum;
    switch (speed)
    {
        case RotarySpeed::Slow:
            horn.targetHz = 0.68f;
            drum.targetHz = 0.58f;
            break;
        case RotarySpeed::Fast:
            horn.targetHz = 6.7f;
            drum.targetHz = 5.8f;
            break;
        default:
            horn.targetHz = 0.0f;
            drum.targetHz = 0.0f;
            break;
    }
}

float LeslieRotarySpeaker::readDelay(const std::vector<float>& line, float delaySamples) const noexcept
{
    const float read = static_cast<float>(writePos) - delaySamples;
    const int i0 = static_cast<int>(std::floor(read));
    const float frac = read - static_cast<float>(i0);
    const float a = line[static_cast<size_t>(i0 & delayMask)];
    const float b = line[static_cast<size_t>((i0 + 1) & delayMask)];
    return a + (b - a) * frac;
}

void LeslieRotarySpeaker::process(juce::AudioBuffer<float>& buffer) noexcept
{
    if (! enabled)
        return;

    const int nCh = juce::jmin(2, buffer.getNumChannels());
    const int n = buffer.getNumSamples();
    if (nCh < 1 || n <= 0)
        return;

    float* L = buffer.getWritePointer(0);
    float* R = nCh > 1 ? buffer.getWritePointer(1) : nullptr;

    const float invSr = 1.0f / static_cast<float>(sampleRate);
    const float twoPi = juce::MathConstants<float>::twoPi;
    const float hornDepth = static_cast<float>(sampleRate * 0.0018); // ~1.8 ms Doppler
    const float drumDepth = static_cast<float>(sampleRate * 0.0026);
    const float baseDelay = static_cast<float>(sampleRate * 0.0035);

    auto slew = [invSr](Rotor& r) noexcept
    {
        const float coeff = (r.targetHz > r.currentHz) ? r.accel : r.decel;
        const float step = coeff * invSr;
        if (r.currentHz < r.targetHz)
            r.currentHz = juce::jmin(r.targetHz, r.currentHz + step);
        else
            r.currentHz = juce::jmax(r.targetHz, r.currentHz - step);
    };

    for (int i = 0; i < n; ++i)
    {
        slew(horn);
        slew(drum);

        horn.angle += horn.currentHz * invSr;
        drum.angle += drum.currentHz * invSr;
        if (horn.angle >= 1.0f) horn.angle -= 1.0f;
        if (drum.angle >= 1.0f) drum.angle -= 1.0f;

        const float hAng = horn.angle * twoPi;
        const float dAng = drum.angle * twoPi;

        const float inL = L[i];
        const float inR = R ? R[i] : inL;
        const float mono = 0.5f * (inL + inR);

        xoverLpL += xoverCoeff * (mono - xoverLpL);
        const float lows = xoverLpL;
        const float highs = mono - xoverLpL;

        hornDelayL[static_cast<size_t>(writePos)] = highs;
        hornDelayR[static_cast<size_t>(writePos)] = highs;
        drumDelayL[static_cast<size_t>(writePos)] = lows;
        drumDelayR[static_cast<size_t>(writePos)] = lows;

        const float hDelL = baseDelay + hornDepth * (0.5f + 0.5f * std::sin(hAng));
        const float hDelR = baseDelay + hornDepth * (0.5f + 0.5f * std::sin(hAng + juce::MathConstants<float>::pi));
        const float dDelL = baseDelay * 1.2f + drumDepth * (0.5f + 0.5f * std::sin(dAng));
        const float dDelR = baseDelay * 1.2f + drumDepth * (0.5f + 0.5f * std::sin(dAng + juce::MathConstants<float>::pi));

        const float hornAmpL = 0.55f + 0.45f * (0.5f + 0.5f * std::cos(hAng));
        const float hornAmpR = 0.55f + 0.45f * (0.5f + 0.5f * std::cos(hAng + juce::MathConstants<float>::pi));
        const float drumAmpL = 0.70f + 0.30f * (0.5f + 0.5f * std::cos(dAng));
        const float drumAmpR = 0.70f + 0.30f * (0.5f + 0.5f * std::cos(dAng + juce::MathConstants<float>::pi));

        float hL = readDelay(hornDelayL, hDelL) * hornAmpL;
        float hR = readDelay(hornDelayR, hDelR) * hornAmpR;
        float dL = readDelay(drumDelayL, dDelL) * drumAmpL;
        float dR = readDelay(drumDelayR, dDelR) * drumAmpR;

        // Mild drum-cabinet colour: one-pole + one-zero around 120 Hz / 2 kHz.
        cabZ1L += 0.12f * (dL - cabZ1L);
        cabZ1R += 0.12f * (dR - cabZ1R);
        dL = dL * 0.65f + cabZ1L * 0.55f;
        dR = dR * 0.65f + cabZ1R * 0.55f;

        const float hornMix = 0.35f + 0.90f * balance;
        const float drumMix = 1.15f - 0.70f * balance;
        L[i] = hL * hornMix + dL * drumMix;
        if (R)
            R[i] = hR * hornMix + dR * drumMix;

        writePos = (writePos + 1) & delayMask;
    }
}
