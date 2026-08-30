#include "VoicingFilters.h"
#include "../Parameters/ParameterLayout.h"

void VoicingFilters::prepare(double sr, int) noexcept
{
    sampleRate = juce::jmax(8000.0, sr);
    reset();
    setFluteCutoff(fluteHz);
    setReedCutoff(reedHz);

    // Integrator leak ~ 40 Hz high-pass equivalent so the saw doesn't wander.
    const float leakHz = 40.0f;
    jenningsLeak = std::exp(-2.0f * juce::MathConstants<float>::pi * leakHz / static_cast<float>(sampleRate));

    const float aaHz = 7200.0f;
    aaCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * aaHz / static_cast<float>(sampleRate));
    aa1.setLP(sampleRate, aaHz);
    aa2.setLP(sampleRate, aaHz);
}

void VoicingFilters::reset() noexcept
{
    flute1 = {};
    flute2 = {};
    reedHp = {};
    aa1.z = 0.0f;
    aa2.z = 0.0f;
    jenningsState = 0.0f;
    setFluteCutoff(fluteHz);
    setReedCutoff(reedHz);
    aa1.setLP(sampleRate, 7200.0f);
    aa2.setLP(sampleRate, 7200.0f);
}

void VoicingFilters::setFluteCutoff(float hz) noexcept
{
    fluteHz = juce::jlimit(200.0f, 8000.0f, hz);
    flute1.setLP(sampleRate, fluteHz);
    flute2.setLP(sampleRate, fluteHz);
}

void VoicingFilters::setReedCutoff(float hz) noexcept
{
    reedHz = juce::jlimit(80.0f, 4000.0f, hz);
    reedHp.setHP(sampleRate, reedHz);
}

void VoicingFilters::setEngineMode(int mode) noexcept
{
    engineMode = mode;

    switch (mode)
    {
        case EngineMode::VoxSuper:
            setFluteCutoff(1250.0f);
            setReedCutoff(280.0f);
            break;
        case EngineMode::JenningsJ70:
            setFluteCutoff(700.0f);
            setReedCutoff(400.0f);
            break;
        case EngineMode::Extended:
            setFluteCutoff(1100.0f);
            setReedCutoff(320.0f);
            break;
        default:
            setFluteCutoff(900.0f);
            setReedCutoff(350.0f);
            break;
    }
}

void VoicingFilters::process(float* data, int numSamples, float fluteAmount, float reedAmount) noexcept
{
    const float fluteGain = juce::jlimit(0.0f, 1.0f, fluteAmount);
    const float reedGain  = juce::jlimit(0.0f, 1.0f, reedAmount);

    const bool jennings = (engineMode == EngineMode::JenningsJ70 || engineMode == EngineMode::Extended);
    const float sawMix = (engineMode == EngineMode::JenningsJ70) ? 0.72f
                        : (engineMode == EngineMode::Extended  ? 0.28f : 0.0f);
    const float squareMix = 1.0f - sawMix * 0.55f;

    const float reedDrive = (engineMode == EngineMode::VoxSuper) ? 2.6f : 2.1f;
    const float superBright = (engineMode == EngineMode::VoxSuper) ? 0.18f : 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        const float raw = data[i];

        float source = raw * squareMix;
        if (jennings)
        {
            jenningsState = jenningsState * jenningsLeak + raw * (1.0f - jenningsLeak);
            source += jenningsState * sawMix * 2.2f;
        }

        const float flute = flute2.lp(flute1.lp(source));
        const float reedHpOut = reedHp.hp(source);
        const float reed = std::tanh(reedHpOut * reedDrive);

        float mixed = flute * fluteGain * 1.15f + reed * reedGain * 0.85f;
        mixed += source * superBright;
        mixed = aa2.lp(aa1.lp(mixed)); // 12 dB/oct anti-alias ~7.2 kHz
        data[i] = mixed;
    }
}
