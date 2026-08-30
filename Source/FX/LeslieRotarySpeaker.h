#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

/**
    Leslie 122/145-style rotary: horn (highs) + drum (lows), Doppler via
    modulated delay, amplitude modulation, stereo, with inertia on
    Stop / Slow / Fast.
*/
class LeslieRotarySpeaker
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset() noexcept;

    void setSpeed(int rotarySpeedEnum) noexcept; // RotarySpeed::Stop/Slow/Fast
    void setEnabled(bool e) noexcept { enabled = e; }
    void setBalance(float b) noexcept { balance = juce::jlimit(0.0f, 1.0f, b); }

    void process(juce::AudioBuffer<float>& buffer) noexcept;

private:
    struct Rotor
    {
        float angle = 0.0f;      // 0..1
        float currentHz = 0.0f;
        float targetHz = 0.0f;
        float accel = 0.0f;      // Hz per second toward target
        float decel = 0.0f;
    };

    float readDelay(const std::vector<float>& line, float delaySamples) const noexcept;

    double sampleRate = 44100.0;
    bool enabled = false;
    int speed = 0;
    float balance = 0.45f;

    Rotor horn, drum;

    std::vector<float> hornDelayL, hornDelayR, drumDelayL, drumDelayR;
    int delayMask = 0;
    int writePos = 0;

    float xoverLpL = 0.0f, xoverLpR = 0.0f;
    float xoverCoeff = 0.0f;

    // Simple 2nd-order cabinet colour on the drum path
    float cabZ1L = 0.0f, cabZ2L = 0.0f, cabZ1R = 0.0f, cabZ2R = 0.0f;
};
